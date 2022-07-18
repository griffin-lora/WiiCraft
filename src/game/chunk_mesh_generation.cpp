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
#include "mesh_generation.hpp"
#include "mesh_generation.inl"
#include <cstdio>

using namespace game;

static constexpr s32 Z_OFFSET = chunk::SIZE * chunk::SIZE;
static constexpr s32 Y_OFFSET = chunk::SIZE;
static constexpr s32 X_OFFSET = 1;

template<block::face face>
static inline std::size_t get_face_index_offset(std::size_t index) {
    return call_face_func_for<face, std::size_t>(
        [&]() { return index + X_OFFSET; },
        [&]() { return index - X_OFFSET; },
        [&]() { return index + Y_OFFSET; },
        [&]() { return index - Y_OFFSET; },
        [&]() { return index + Z_OFFSET; },
        [&]() { return index - Z_OFFSET; }
    );
}

template<typename Bf, block::face face, typename Vf>
static void add_face_vertices_if_needed(const block* blocks, const block* nb_blocks, std::size_t index, bool should_render_face, bl_st block_state, Vf& vf, math::vector3u8 block_pos) {
    if (
        Bf::template get_face_traits<face>(block_state).visible &&
        should_render_face &&
        Bf::template is_face_visible_with_neighbor<face>(block_state, blocks[get_face_index_offset<face>(index)])
    ) {
        Bf::template add_face_vertices<face>(vf, block_pos, block_state);
    }
}

void game::update_mesh(standard_quad_building_arrays& building_arrays, chunk& chunk) {
    const auto blocks = chunk.blocks.data();
    
    const auto& chunk_nh = chunk.nh;

    auto get_nb_blocks = [](chunk::const_opt_ref nb_chunk) -> const block* {
        if (nb_chunk.has_value()) {
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

    standard_vertex_function vf = {
        .it = { building_arrays }
    };

    standard_quad_iterators begin = { building_arrays };

    std::size_t index = 0;
    for (u32 z = 0; z < chunk::SIZE; z++) {
        bool should_render_left = z != 0;
        bool should_render_right = z != (chunk::SIZE - 1);

        for (u32 y = 0; y < chunk::SIZE; y++) {
            bool should_render_bottom = y != 0;
            bool should_render_top = y != (chunk::SIZE - 1);

            for (u32 x = 0; x < chunk::SIZE; x++) {
                bool should_render_back = x != 0;
                bool should_render_front = x != (chunk::SIZE - 1);

                auto& block = blocks[index];

                auto visible = get_with_block_functionality<bool>(block.tp, [&block]<typename Bf>() { return Bf::get_block_traits(block.st).visible; });

                if (visible) {
                    call_with_block_functionality(block.tp, [&]<typename Bf>() {
                        math::vector3u8 block_pos = { x, y, z };

                        #define EVAL_CALL_FACE_VERTICES_IF_NEEDED(uppercase, lowercase, axis) add_face_vertices_if_needed<Bf, block::face::uppercase>(blocks, lowercase##_nb_blocks, index, should_render_##lowercase, block.st, vf, block_pos);

                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(FRONT, front, x)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(BACK, back, x)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(TOP, top, y)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(BOTTOM, bottom, y)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(RIGHT, right, z)
                        EVAL_CALL_FACE_VERTICES_IF_NEEDED(LEFT, left, z)

                        Bf::add_general_vertices(vf, block_pos, block.st);
                    });
                }

                if (
                    (vf.it.standard - begin.standard) > chunk::MAX_STANDARD_QUAD_COUNT ||
                    (vf.it.foliage - begin.foliage) > chunk::MAX_FOLIAGE_QUAD_COUNT ||
                    (vf.it.water - begin.water) > chunk::MAX_WATER_QUAD_COUNT
                ) {
                    dbg::error([]() {
                        printf("Chunk quad count is too high\n");
                    });
                }

                index += X_OFFSET;
            }
        }
    }

    write_into_display_lists(begin, vf, chunk.standard_disp_list, chunk.foliage_disp_list, chunk.water_disp_list, [](auto vert_count) {
        return (
            (vert_count > 0xff ? 4 : 3) + // GX_Begin
            vert_count * 3 + // GX_Position3u8
            vert_count * 2 // GX_TexCoord2u8
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
        GX_TexCoord2u8(vert.uv.x, vert.uv.y);
    });
}