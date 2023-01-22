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
            }, (game::block::state)game::block::slab_state::bottom, block_pos);
        });
    }
}

#define NUM_BUILDING_QUADS 0x800

typedef struct chunk_mesh_vertex {
    u8 x;
    u8 y;
    u8 z;
    u8 u;
    u8 v;
} chunk_mesh_vertex_t;

typedef struct {
    chunk_mesh_vertex_t verts[4];
} chunk_mesh_quad_t;

static_assert(sizeof(chunk_mesh_quad_t) == 4 * 5);

alignas(32768) static chunk_mesh_quad_t building_quads[NUM_BUILDING_QUADS];

static void check_quads_count(size_t quads_count) {
    if (quads_count >= NUM_BUILDING_QUADS) [[unlikely]] {
        dbg::error([quads_count]() {
            std::printf("Too many quads for should be %d, count is %d\n", NUM_BUILDING_QUADS, quads_count);
        });
    }
}

mesh_update_state game::update_core_mesh(chunk_quad_building_arrays& _, chunk& chunk) {
    if (
        chunk.invisible_block_count == chunk::blocks_count ||
        chunk.fully_opaque_block_count == chunk::blocks_count
    ) {
        chunk.disp_list.clear();
        return mesh_update_state::should_continue;
    }

    const block* blocks = chunk.blocks.data();

    size_t quads_index = 0;

    // Generate mesh for faces that are not neighboring another chunk.
    size_t blocks_index = 0;
    for (u32 z = 0; z < chunk::size; z++) {
        bool should_add_left = z != 0;
        bool should_add_right = z != (chunk::size - 1);

        for (u32 y = 0; y < chunk::size; y++) {
            bool should_add_bottom = y != 0;
            bool should_add_top = y != (chunk::size - 1);

            for (u32 x = 0; x < chunk::size; x++) {
                bool should_add_back = x != 0;
                bool should_add_front = x != (chunk::size - 1);

                block block = blocks[blocks_index];
                math::vector3u8 block_pos = { x, y, z };

                if (block.tp == block::type::grass) {
                    u8 px = x * 4;
                    u8 py = y * 4;
                    u8 pz = z * 4;
                    u8 pox = px + 4;
                    u8 poy = py + 4;
                    u8 poz = pz + 4;
                    u8 ux = 0;
                    u8 uy = 0;
                    u8 uox = 4;
                    u8 uoy = 4;

                    building_quads[quads_index++] = chunk_mesh_quad_t{{
                        { px, poy, poz, ux, uy },	// Bottom Left Of The Quad (Back)
                        { px, poy, pz, uox, uy },	// Bottom Right Of The Quad (Back)
                        { pox, poy, pz, uox, uoy },	// Top Right Of The Quad (Back)
                        { pox, poy, poz, ux, uoy }	// Top Left Of The Quad (Back)
                    }};
                }

                check_quads_count(quads_index);

                blocks_index++;
            }
        }
    }

    size_t num_quads_written = quads_index;
    size_t num_verts_written = num_quads_written * 4;

    chunk.disp_list.resize(
        gfx::get_begin_instruction_size(num_verts_written) +
        gfx::get_vector_instruction_size<3, u8>(num_verts_written) + // Position
        gfx::get_vector_instruction_size<2, u8>(num_verts_written) // Tex Coords
    );
    chunk.disp_list.write_into([num_quads_written, num_verts_written]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, num_verts_written);

        for (size_t i = 0; i < num_quads_written; i++) {
            chunk_mesh_quad_t* quad = &building_quads[i];
            
            GX_Position3u8(quad->verts[0].x, quad->verts[0].y, quad->verts[0].z);
            GX_TexCoord2u8(quad->verts[0].u, quad->verts[0].v);

            GX_Position3u8(quad->verts[1].x, quad->verts[1].y, quad->verts[1].z);
            GX_TexCoord2u8(quad->verts[1].u, quad->verts[1].v);

            GX_Position3u8(quad->verts[2].x, quad->verts[2].y, quad->verts[2].z);
            GX_TexCoord2u8(quad->verts[2].u, quad->verts[2].v);

            GX_Position3u8(quad->verts[3].x, quad->verts[3].y, quad->verts[3].z);
            GX_TexCoord2u8(quad->verts[3].u, quad->verts[3].v);
        }
        
        GX_End();
    });

    return mesh_update_state::should_break;
}

mesh_update_state game::update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk) {
    return mesh_update_state::should_break;
}