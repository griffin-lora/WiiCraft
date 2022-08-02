#include "chunk_mesh_generation.hpp"
#include "block_core.hpp"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include "block_functionality.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"
#include <cstdio>

using namespace game;
struct iterator_container {
    template<typename T>
    using type = T::iterator;
};

struct chunk_quad_iterators : public block_mesh_layers<chunk_quad_array_iterator_container> {
    chunk_quad_iterators(chunk_quad_building_arrays& arrays) : block_mesh_layers<chunk_quad_array_iterator_container>([&arrays]<typename L>() { return arrays.get_layer<L>().begin(); }) {}
};

struct chunk_mesh_state {
    chunk_quad_iterators it;

    template<typename L>
    inline void add_quad(const L::chunk_quad& quad) {
        *it.get_layer<L>()++ = quad;
    }
};

template<typename F1, typename F2, typename I>
static void write_into_display_list(F1 get_disp_list_size, F2 write_vert, I begin, I end, gfx::display_list& disp_list) {
    std::size_t vert_count = (end - begin) * 4;

    disp_list.resize(get_disp_list_size(vert_count));

    disp_list.write_into([&write_vert, &begin, &end, vert_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vert_count);

        for (auto it = begin; it != end; ++it) {
            write_vert(it->vert0);
            write_vert(it->vert1);
            write_vert(it->vert2);
            write_vert(it->vert3);
        }
        
        GX_End();
    });
};

static void write_into_display_list_layers(const chunk_quad_iterators& begin, const chunk_quad_iterators& end, chunk::display_list_layers& disp_list_layers) {
    for_each_block_mesh_layer([&begin, &end, &disp_list_layers]<typename L>() {
        write_into_display_list(L::get_chunk_display_list_size, L::write_chunk_vertex, begin.get_layer<L>(), end.get_layer<L>(), disp_list_layers.get_layer<L>());
    });
}

static constexpr s32 z_offset = chunk::size * chunk::size;
static constexpr s32 y_offset = chunk::size;
static constexpr s32 x_offset = 1;

using const_block_it = ext::data_array<block>::const_iterator;

template<block::face FACE>
static inline const_block_it get_block_face_iterator_offset(const_block_it it) {
    return call_face_func_for<FACE, const_block_it>(
        [&]() { return it + x_offset; },
        [&]() { return it - x_offset; },
        [&]() { return it + y_offset; },
        [&]() { return it - y_offset; },
        [&]() { return it + z_offset; },
        [&]() { return it - z_offset; }
    );
}

template<block::face FACE>
static void add_face_vertices_if_needed_at_neighbor(const block* blocks, const block* nb_blocks, std::size_t index, std::size_t nb_chunk_index, chunk_mesh_state& ms_st, math::vector3u8 block_pos) {
    if (nb_blocks != nullptr) {
        auto& bl = blocks[index];
        call_with_block_functionality(bl.tp, [&]<typename BF>() {
            add_block_faces_vertices<BF>(ms_st, [nb_blocks, nb_chunk_index]<block::face GET_NB_FACE>() -> const block* { // Get neighbor block
                if constexpr (GET_NB_FACE == FACE) {
                    return &nb_blocks[nb_chunk_index];
                } else {
                    return nullptr;
                }
            }, bl.st, block_pos);
        });
    }
}

static void check_vertex_count(const chunk_quad_iterators& begin, const chunk_quad_iterators& end) {
    for_each_block_mesh_layer([&begin, &end]<typename L>() {
        std::size_t quad_count = end.get_layer<L>() - begin.get_layer<L>();
        if (quad_count >= L::max_quad_count) {
            dbg::error([quad_count]() {
                std::printf("Too many quads for %s, should be %d, count is %d\n", L::name, L::max_quad_count, quad_count);
            });
        }
    });
}

static inline void clear_display_list_layers(chunk::display_list_layers& disp_list_layers) {
    for_each_block_mesh_layer([&disp_list_layers]<typename L>() {
        disp_list_layers.get_layer<L>().clear();
    });
}

mesh_update_state game::update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk) {
    if (
        chunk.invisible_block_count == chunk::blocks_count ||
        chunk.fully_opaque_block_count == chunk::blocks_count
    ) {
        clear_display_list_layers(chunk.core_disp_list_layers);
        return mesh_update_state::should_continue;
    }

    const chunk_quad_iterators begin = { building_arrays };

    chunk_mesh_state ms_st = {
        .it = { building_arrays }
    };

    // Generate mesh for faces that are not neighboring another chunk.
    auto it = chunk.blocks.begin();
    for (u32 z = 0; z < chunk::size; z++) {
        bool should_add_left = z != 0;
        bool should_add_right = z != (chunk::size - 1);

        for (u32 y = 0; y < chunk::size; y++) {
            bool should_add_bottom = y != 0;
            bool should_add_top = y != (chunk::size - 1);

            for (u32 x = 0; x < chunk::size; x++) {
                bool should_add_back = x != 0;
                bool should_add_front = x != (chunk::size - 1);

                auto& bl = *it;
                math::vector3u8 block_pos = { x, y, z };

                call_with_block_functionality(bl.tp, [&]<typename BF>() {
                    add_block_vertices<BF>(ms_st, [
                        should_add_left,
                        should_add_right,
                        should_add_bottom,
                        should_add_top,
                        should_add_back,
                        should_add_front,
                        it
                    ]<block::face FACE>() -> const block* { // Get neighbor block
                        bool should_add_face = call_face_func_for<FACE, bool>(
                            [&]() { return should_add_front; },
                            [&]() { return should_add_back; },
                            [&]() { return should_add_top; },
                            [&]() { return should_add_bottom; },
                            [&]() { return should_add_right; },
                            [&]() { return should_add_left; }
                        );
                        if (should_add_face) {
                            return &(*get_block_face_iterator_offset<FACE>(it));
                        } else {
                            return nullptr;
                        }
                    }, bl.st, block_pos);
                });

                check_vertex_count(begin, ms_st.it);

                it += x_offset;
            }
        }
    }

    write_into_display_list_layers(begin, ms_st.it, chunk.core_disp_list_layers);
    
    return mesh_update_state::should_break;
}

mesh_update_state game::update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk) {
    if (chunk.invisible_block_count == chunk::blocks_count) {
        clear_display_list_layers(chunk.shell_disp_list_layers);
        return mesh_update_state::should_continue;
    }

    const auto blocks = chunk.blocks.data();
    
    const auto& chunk_nh = chunk.nh;

    auto get_nb_blocks = [](chunk::const_opt_ref nb_chunk) -> const block* {
        if (nb_chunk.has_value()) {
            if (nb_chunk->get().fully_opaque_block_count == chunk::blocks_count) {
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

    // TODO: Implement this better.
    if (front_nb_blocks == nullptr && back_nb_blocks == nullptr && top_nb_blocks == nullptr && bottom_nb_blocks == nullptr && right_nb_blocks == nullptr && left_nb_blocks == nullptr) {
        clear_display_list_layers(chunk.shell_disp_list_layers);
        return mesh_update_state::should_continue;
    }

    const chunk_quad_iterators begin = { building_arrays };

    chunk_mesh_state ms_st = {
        .it = { building_arrays }
    };
    
    // Generate mesh for faces that are neighboring another chunk.
    std::size_t front_index = chunk::size - 1;
    std::size_t back_index = 0;

    std::size_t top_index = (chunk::size - 1) * y_offset;
    std::size_t bottom_index = 0;

    std::size_t right_index = (chunk::size - 1) * z_offset;
    std::size_t left_index = 0;

    for (u32 far = 0; far < chunk::size; far++) {
        for (u32 near = 0; near < chunk::size; near++) {
            add_face_vertices_if_needed_at_neighbor<block::face::front>(blocks, front_nb_blocks, front_index, back_index, ms_st, { chunk::size - 1, near, far });
            add_face_vertices_if_needed_at_neighbor<block::face::back>(blocks, back_nb_blocks, back_index, front_index, ms_st, { 0, near, far });
            add_face_vertices_if_needed_at_neighbor<block::face::top>(blocks, top_nb_blocks, top_index, bottom_index, ms_st, { near, chunk::size - 1, far });
            add_face_vertices_if_needed_at_neighbor<block::face::bottom>(blocks, bottom_nb_blocks, bottom_index, top_index, ms_st, { near, 0, far });
            add_face_vertices_if_needed_at_neighbor<block::face::right>(blocks, right_nb_blocks, right_index, left_index, ms_st, { near, far, chunk::size - 1 });
            add_face_vertices_if_needed_at_neighbor<block::face::left>(blocks, left_nb_blocks, left_index, right_index, ms_st, { near, far, 0 });
            
            check_vertex_count(begin, ms_st.it);
            
            front_index += y_offset;
            back_index += y_offset;

            top_index += x_offset;
            bottom_index += x_offset;

            right_index += x_offset;
            left_index += x_offset;
        }

        top_index += z_offset - y_offset;
        bottom_index += z_offset - y_offset;
    }

    write_into_display_list_layers(begin, ms_st.it, chunk.shell_disp_list_layers);

    return mesh_update_state::should_break;
}