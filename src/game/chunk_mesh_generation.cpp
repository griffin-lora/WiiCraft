#include "chunk_mesh_generation.hpp"
#include "block_core.hpp"
#include "block_core.inl"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "block_mesh_generation.hpp"
#include "block_mesh_generation.inl"
#include <cstdio>

using namespace game;

static constexpr s32 Z_OFFSET = chunk::SIZE * chunk::SIZE;
static constexpr s32 Y_OFFSET = chunk::SIZE;
static constexpr s32 X_OFFSET = 1;

using const_block_it = ext::data_array<block>::const_iterator;

template<block::face face>
static inline const_block_it get_block_face_iterator_offset(const_block_it it) {
    return call_face_func_for<face, const_block_it>(
        [&]() { return it + X_OFFSET; },
        [&]() { return it - X_OFFSET; },
        [&]() { return it + Y_OFFSET; },
        [&]() { return it - Y_OFFSET; },
        [&]() { return it + Z_OFFSET; },
        [&]() { return it - Z_OFFSET; }
    );
}

template<typename Bf, block::face face>
static void add_face_vertices_if_needed(const_block_it it, bool should_render_face, bl_st block_state, block_mesh_state& ms_st, math::vector3u8 block_pos) {
    if (
        Bf::template get_face_traits<face>(block_state).visible &&
        should_render_face &&
        Bf::template is_face_visible_with_neighbor<face>(block_state, *get_block_face_iterator_offset<face>(it))
    ) {
        Bf::template add_face_vertices<face>(ms_st, block_pos, block_state);
    }
}

static bool is_block_visible(const block& block) {
    return get_with_block_functionality<bool>(block.tp, [&block]<typename Bf>() { return Bf::get_block_traits(block.st).visible; });
}

template<block::face face, typename F>
static void add_face_vertices_if_needed_at_neighbor(const block* blocks, const block* nb_blocks, std::size_t index, std::size_t nb_chunk_index, block_mesh_state& ms_st, F get_block_pos) {
    if (nb_blocks != nullptr) {
        auto& block = blocks[index];
        if (is_block_visible(block)) {
            call_with_block_functionality(block.tp, [&]<typename Bf>() {
                math::vector3u8 block_pos = get_block_pos();

                if (
                    Bf::template get_face_traits<face>(block.st).visible &&
                    Bf::template is_face_visible_with_neighbor<face>(block.st, nb_blocks[nb_chunk_index])
                ) {
                    Bf::template add_face_vertices<face>(ms_st, block_pos, block.st);
                }
            });
        }
    }
}

static void check_vertex_count(const block_quad_iterators& begin, const block_quad_iterators& end) {
    if (
        (end.standard - begin.standard) > chunk::MAX_STANDARD_QUAD_COUNT ||
        (end.foliage - begin.foliage) > chunk::MAX_FOLIAGE_QUAD_COUNT ||
        (end.water - begin.water) > chunk::MAX_WATER_QUAD_COUNT
    ) {
        dbg::error([]() {
            printf("Chunk quad count is too high\n");
        });
    }
}

static inline void write_into_chunk_display_lists(const block_quad_iterators& begin, const block_quad_iterators& end, chunk::display_lists& disp_lists) {
    write_into_display_lists(begin, end, disp_lists.standard, disp_lists.foliage, disp_lists.water, [](auto vert_count) {
        return (
            gfx::get_begin_instruction_size(vert_count) +
            gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
            gfx::get_vector_instruction_size<2, u8>(vert_count) // UV
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
        GX_TexCoord2u8(vert.uv.x, vert.uv.y);
    });
}

void game::update_core_mesh(block_quad_building_arrays& building_arrays, chunk& chunk) {
    if (
        chunk.invisible_block_count == chunk::BLOCKS_COUNT ||
        chunk.fully_transparent_block_count == chunk::BLOCKS_COUNT
    ) {
        return;
    }

    const block_quad_iterators begin = { building_arrays };

    block_mesh_state ms_st = {
        .it = { building_arrays }
    };

    // Generate mesh for faces that are not neighboring another chunk.
    auto it = chunk.blocks.begin();
    for (u32 z = 0; z < chunk::SIZE; z++) {
        bool should_render_left = z != 0;
        bool should_render_right = z != (chunk::SIZE - 1);

        for (u32 y = 0; y < chunk::SIZE; y++) {
            bool should_render_bottom = y != 0;
            bool should_render_top = y != (chunk::SIZE - 1);

            for (u32 x = 0; x < chunk::SIZE; x++) {
                bool should_render_back = x != 0;
                bool should_render_front = x != (chunk::SIZE - 1);

                auto& block = *it;

                if (is_block_visible(block)) {
                    call_with_block_functionality(block.tp, [&]<typename Bf>() {
                        math::vector3u8 block_pos = { x, y, z };

                        #define EVAL_CALL_FACE_VERTICES_IF_NEEDED(uppercase, lowercase, axis) add_face_vertices_if_needed<Bf, block::face::uppercase>(it, should_render_##lowercase, block.st, ms_st, block_pos);

                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(FRONT, front, x)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(BACK, back, x)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(TOP, top, y)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(BOTTOM, bottom, y)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(RIGHT, right, z)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(LEFT, left, z)

                        Bf::add_general_vertices(ms_st, block_pos, block.st);
                    });
                }

                check_vertex_count(begin, ms_st.it);

                it += X_OFFSET;
            }
        }
    }

    write_into_chunk_display_lists(begin, ms_st.it, chunk.core_disp_lists);
}

void game::update_shell_mesh(block_quad_building_arrays& building_arrays, chunk& chunk) {
    if (chunk.invisible_block_count == chunk::BLOCKS_COUNT) {
        return;
    }

    const auto blocks = chunk.blocks.data();
    
    const auto& chunk_nh = chunk.nh;

    auto get_nb_blocks = [](chunk::const_opt_ref nb_chunk) -> const block* {
        if (nb_chunk.has_value()) {
            if (
                nb_chunk->get().invisible_block_count == chunk::BLOCKS_COUNT ||
                nb_chunk->get().fully_transparent_block_count == chunk::BLOCKS_COUNT
            ) {
                return nullptr;
            }
            return nb_chunk->get().blocks.data();
        }
        return nullptr;
    };

    auto front_nb_blocks = get_nb_blocks(chunk_nh.front);
    auto back_nb_blocks = get_nb_blocks(chunk_nh.back);
    auto top_nb_blocks = get_nb_blocks(chunk_nh.top);
    auto bottom_nb_blocks = get_nb_blocks(chunk_nh.bottom);
    auto right_nb_blocks = get_nb_blocks(chunk_nh.right);
    auto left_nb_blocks = get_nb_blocks(chunk_nh.left);

    const block_quad_iterators begin = { building_arrays };

    block_mesh_state ms_st = {
        .it = { building_arrays }
    };
    
    // Generate mesh for faces that are neighboring another chunk.
    std::size_t front_index = chunk::SIZE - 1;
    std::size_t back_index = 0;

    std::size_t top_index = (chunk::SIZE - 1) * Y_OFFSET;
    std::size_t bottom_index = 0;

    std::size_t right_index = (chunk::SIZE - 1) * Z_OFFSET;
    std::size_t left_index = 0;

    for (u32 far = 0; far < chunk::SIZE; far++) {
        for (u32 near = 0; near < chunk::SIZE; near++) {
            add_face_vertices_if_needed_at_neighbor<block::face::FRONT>(blocks, front_nb_blocks, front_index, back_index, ms_st, [far, near]() {
                return math::vector3u8{ chunk::SIZE - 1, near, far };
            });
            add_face_vertices_if_needed_at_neighbor<block::face::BACK>(blocks, back_nb_blocks, back_index, front_index, ms_st, [far, near]() {
                return math::vector3u8{ 0, near, far };
            });
            add_face_vertices_if_needed_at_neighbor<block::face::TOP>(blocks, top_nb_blocks, top_index, bottom_index, ms_st, [far, near]() {
                return math::vector3u8{ near, chunk::SIZE - 1, far };
            });
            add_face_vertices_if_needed_at_neighbor<block::face::BOTTOM>(blocks, bottom_nb_blocks, bottom_index, top_index, ms_st, [far, near]() {
                return math::vector3u8{ near, 0, far };
            });
            add_face_vertices_if_needed_at_neighbor<block::face::RIGHT>(blocks, right_nb_blocks, right_index, left_index, ms_st, [far, near]() {
                return math::vector3u8{ near, far, chunk::SIZE - 1 };
            });
            add_face_vertices_if_needed_at_neighbor<block::face::LEFT>(blocks, left_nb_blocks, left_index, right_index, ms_st, [far, near]() {
                return math::vector3u8{ near, far, 0 };
            });
            
            check_vertex_count(begin, ms_st.it);
            
            front_index += Y_OFFSET;
            back_index += Y_OFFSET;

            top_index += X_OFFSET;
            bottom_index += X_OFFSET;

            right_index += X_OFFSET;
            left_index += X_OFFSET;
        }

        top_index += Z_OFFSET - Y_OFFSET;
        bottom_index += Z_OFFSET - Y_OFFSET;
    }

    write_into_chunk_display_lists(begin, ms_st.it, chunk.shell_disp_lists);
}