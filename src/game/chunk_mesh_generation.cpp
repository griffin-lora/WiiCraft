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

typedef block::type block_type_t;

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

typedef enum {
    block_mesh_category_invisible,
    block_mesh_category_cube,
    block_mesh_category_cross,
    block_mesh_category_slab_bottom,
    block_mesh_category_slab_top
} block_mesh_category_t;

static block_mesh_category_t get_block_mesh_category(block_type_t type) {
    switch (type) {
        default:
            return block_mesh_category_invisible;
        case block::type::debug:
        case block::type::grass:
        case block::type::stone:
        case block::type::dirt:
        case block::type::sand:
        case block::type::wood_planks:
        case block::type::stone_slab_both:
            return block_mesh_category_cube;
        case block::type::tall_grass:
            return block_mesh_category_cross;
        case block::type::stone_slab_bottom:
            return block_mesh_category_slab_bottom;
        case block::type::stone_slab_top:
            return block_mesh_category_slab_top;
    }
}

#define NUM_BLOCKS (32 * 32 * 32)
#define Z_OFFSET (32 * 32)
#define Y_OFFSET 32
#define X_OFFSET 1

typedef size_t (*add_face_mesh_function_t)(size_t, u32, u32, u32);

static size_t add_cube_face_mesh_if_needed(const block_type_t block_types[], size_t quads_index, u32 x, u32 y, u32 z, bool should_add_face, size_t neighbor_index, add_face_mesh_function_t add_face_mesh_function) {
    if (!should_add_face) [[unlikely]] {
        return quads_index;
    }
    block_mesh_category_t neighbor_mesh_category = get_block_mesh_category(block_types[neighbor_index]);
    if (neighbor_mesh_category == block_mesh_category_cube) {
        return quads_index;
    }
    return add_face_mesh_function(quads_index, x, y, z);
}

/*
The idea of how I'm going to optimize this is to layout the cache like this
[
block types
-
-
-
display lists
]
This will avoid any fighting over space in the cache and will mean that the amount of unnecessary cache misses will be 0
I am also going to try to optimize this by not using the stack anywhere within the update_core_mesh function since this will mean that I can position the block memory and display lists anywhere in the cache without having to worry about where the stack will be in the cache, should that not be possible I will use a layout roughly like this
[
block types
-
-
stack
-
-
display lists
]

Ways to optimize for avoiding stack usage will be to not use the should_add_face loop computed values. The original intent behind this optimization was to reduce the number of comparison operations, however it is the branch that is expensive and so this isnt really a worthwhile optimization and takes up valuable register space.
*/

mesh_update_state game::update_core_mesh(chunk_quad_building_arrays& _, chunk& chunk) {
    if (
        chunk.invisible_block_count == chunk::blocks_count ||
        chunk.fully_opaque_block_count == chunk::blocks_count
    ) {
        chunk.disp_list.clear();
        return mesh_update_state::should_continue;
    }

    const block_type_t* block_types = (const block_type_t*)chunk.blocks.data();

    size_t quads_index = 0;

    // Generate mesh for faces that are not neighboring another chunk.
    size_t blocks_index = 0;
    for (u32 z = 0; z < chunk::size; z++) {
        bool should_add_right = z != (chunk::size - 1);
        bool should_add_left = z != 0;

        for (u32 y = 0; y < chunk::size; y++) {
            bool should_add_top = y != (chunk::size - 1);
            bool should_add_bottom = y != 0;

            for (u32 x = 0; x < chunk::size; x++) {
                bool should_add_front = x != (chunk::size - 1);
                bool should_add_back = x != 0;
                
                block_type_t type = block_types[blocks_index];

                block_mesh_category_t mesh_category = get_block_mesh_category(type);
                switch (mesh_category) {
                    default: break;
                    case block_mesh_category_cube: {
                        quads_index = add_cube_face_mesh_if_needed(block_types, quads_index, x, y, z, should_add_top, blocks_index + Y_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z) {
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

                            return quads_index;
                        });

                        break;
                    }
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