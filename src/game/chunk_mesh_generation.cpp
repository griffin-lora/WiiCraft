#include "chunk_mesh_generation.hpp"
#include "block_core.hpp"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include "block_functionality.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"
#include "log.hpp"
#include <cstdio>

using namespace game;

typedef block::type block_type_t;
typedef block::face block_face_t;

#define NUM_BUILDING_QUADS 0x1000

typedef struct chunk_mesh_vertex {
    u8 px;
    u8 py;
    u8 pz;
    u8 tx;
    u8 ty;
} chunk_mesh_vertex_t;

typedef struct {
    chunk_mesh_vertex_t verts[4];
} chunk_mesh_quad_t;

static_assert(sizeof(chunk_mesh_quad_t) == 4 * 5);

alignas(32768) static chunk_mesh_quad_t building_quads[NUM_BUILDING_QUADS + 0x20];

static void check_num_quads_written(size_t num_quads_written) {
    if (num_quads_written >= NUM_BUILDING_QUADS) [[unlikely]] {
        // dbg::error([num_quads_written]() {
        //     std::printf("Too many quads for should be %d, count is %d\n", NUM_BUILDING_QUADS, num_quads_written);
        // });
        dbg::freeze();
    }
}

typedef enum {
    block_mesh_category_invisible,
    block_mesh_category_cube,
    block_mesh_category_transparent_cube,
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
        case block::type::water:
            return block_mesh_category_transparent_cube;
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

typedef size_t (*add_face_mesh_function_t)(size_t, u32, u32, u32, block_type_t);

static size_t add_cube_face_mesh_if_needed(size_t quads_index, const block_type_t block_types[], u32 x, u32 y, u32 z, block_type_t type, size_t neighbor_index, add_face_mesh_function_t add_face_mesh_function) {
    // TODO: This check doesn't work
    if (neighbor_index >= NUM_BLOCKS) [[unlikely]] {
        return quads_index;
    }
    block_mesh_category_t neighbor_mesh_category = get_block_mesh_category(block_types[neighbor_index]);
    if (neighbor_mesh_category == block_mesh_category_cube) {
        return quads_index;
    }
    return add_face_mesh_function(quads_index, x, y, z, type);
}

static u8 get_cube_face_tx(block_type_t type, block_face_t face) {
    switch (type) {
        case block::type::debug: return 0;
        case block::type::grass: switch (face) {
            default: return 16;
            case block::face::top: return 0;
            case block::face::bottom: return 8;
        }
        case block::type::stone: return 4;
        case block::type::dirt: return 8;
        case block::type::sand: return 24;
        case block::type::wood_planks: return 40;
        case block::type::stone_slab_both: switch (face) {
            default: return 32;
            case block::face::top: return 36;
        }
    }
}

static u8 get_cube_face_ty(block_type_t type, block_face_t face) {
    return 0;
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

// Possible future optimization is to stop generating the mesh after we reach the end of blocks to generate meshes from

static size_t generate_block_meshes_into_building_mesh(const block_type_t block_types[]) {
    size_t quads_index = 0;

    // Generate mesh for faces that are not neighboring another chunk.
    size_t blocks_index = 0;
    for (u32 z = 0; z < chunk::size; z++) {
        for (u32 y = 0; y < chunk::size; y++) {
            for (u32 x = 0; x < chunk::size; x++) {
                block_type_t type = block_types[blocks_index];

                block_mesh_category_t mesh_category = get_block_mesh_category(type);
                switch (mesh_category) {
                    default: break;
                    case block_mesh_category_cube: {
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index + X_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::front);
                            u8 ty = get_cube_face_ty(type, block::face::front);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                { pox,poy, pz, tx,ty },
                                { pox,py, pz, tx, uoy },
                                { pox,py,poz, uox,uoy },
                                { pox,poy,poz, uox, ty }
                            }};

                            return quads_index;
                        });
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index - X_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::back);
                            u8 ty = get_cube_face_ty(type, block::face::back);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                {  px, poy, pz, tx,ty },	// Top Left of the quad (top)
                                { px, poy, poz, uox,ty },	// Top Right of the quad (top)
                                { px, py, poz, uox,uoy },	// Bottom Right of the quad (top)
                                { px, py, pz, tx,uoy }		// Bottom Left of the quad (top)
                            }};

                            return quads_index;
                        });
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index + Y_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::top);
                            u8 ty = get_cube_face_ty(type, block::face::top);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                { px, poy, poz, tx, ty },	// Bottom Left Of The Quad (Back)
                                { px, poy, pz, uox, ty },	// Bottom Right Of The Quad (Back)
                                { pox, poy, pz, uox, uoy },	// Top Right Of The Quad (Back)
                                { pox, poy, poz, tx, uoy }	// Top Left Of The Quad (Back)
                            }};

                            return quads_index;
                        });
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index - Y_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::bottom);
                            u8 ty = get_cube_face_ty(type, block::face::bottom);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                { px, py, poz, tx, ty },		// Top Right Of The Quad (Front)
                                { pox, py, poz, uox, ty },	// Top Left Of The Quad (Front)
                                { pox, py, pz, uox, uoy },	// Bottom Left Of The Quad (Front)
                                { px, py, pz, tx, uoy }	// Bottom Right Of The Quad (Front)
                            }};

                            return quads_index;
                        });
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index + Z_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::right);
                            u8 ty = get_cube_face_ty(type, block::face::right);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                { pox, py,poz, tx,uoy },	// Top Right Of The Quad (Right)
                                { px, py, poz, uox,uoy },		// Top Left Of The Quad (Right)
                                { px,poy, poz, uox,ty },	// Bottom Left Of The Quad (Right)
                                { pox,poy,poz, tx,ty }	// Bottom Right Of The Quad (Right)
                            }};

                            return quads_index;
                        });
                        quads_index = add_cube_face_mesh_if_needed(quads_index, block_types, x, y, z, type, blocks_index - Z_OFFSET, [](size_t quads_index, u32 x, u32 y, u32 z, block_type_t type) {
                            u8 px = x * 4;
                            u8 py = y * 4;
                            u8 pz = z * 4;
                            u8 pox = px + 4;
                            u8 poy = py + 4;
                            u8 poz = pz + 4;
                            u8 tx = get_cube_face_tx(type, block::face::left);
                            u8 ty = get_cube_face_ty(type, block::face::left);
                            u8 uox = tx + 4;
                            u8 uoy = ty + 4;

                            building_quads[quads_index++] = chunk_mesh_quad_t{{
                                { pox, py, pz, tx,uoy },	// Top Right Of The Quad (Left)
                                { pox, poy,pz, tx,ty },	// Top Left Of The Quad (Left)
                                { px,poy,pz, uox,ty },	// Bottom Left Of The Quad (Left)
                                { px,py, pz, uox,uoy }	// Bottom Right Of The Quad (Left)
                            }};

                            return quads_index;
                        });

                        break;
                    }
                }

                check_num_quads_written(quads_index);

                blocks_index++;
            }
        }
    }

    return quads_index;
}

static void write_building_mesh_into_display_list(size_t num_quads_written, gfx::display_list* disp_list) {
    size_t num_verts_written = num_quads_written * 4;
    disp_list->resize(
        gfx::get_begin_instruction_size(num_verts_written) +
        gfx::get_vector_instruction_size<3, u8>(num_verts_written) + // Position
        gfx::get_vector_instruction_size<2, u8>(num_verts_written) // Tex Coords
    );
    disp_list->write_into([num_quads_written, num_verts_written]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, num_verts_written);

        for (size_t i = 0; i < num_quads_written; i++) {
            chunk_mesh_quad_t* quad = &building_quads[i];
            
            GX_Position3u8(quad->verts[0].px, quad->verts[0].py, quad->verts[0].pz);
            GX_TexCoord2u8(quad->verts[0].tx, quad->verts[0].ty);

            GX_Position3u8(quad->verts[1].px, quad->verts[1].py, quad->verts[1].pz);
            GX_TexCoord2u8(quad->verts[1].tx, quad->verts[1].ty);

            GX_Position3u8(quad->verts[2].px, quad->verts[2].py, quad->verts[2].pz);
            GX_TexCoord2u8(quad->verts[2].tx, quad->verts[2].ty);

            GX_Position3u8(quad->verts[3].px, quad->verts[3].py, quad->verts[3].pz);
            GX_TexCoord2u8(quad->verts[3].tx, quad->verts[3].ty);
        }
        
        GX_End();
    });
}

mesh_update_state game::update_core_mesh(chunk_quad_building_arrays& _, chunk& chunk) {
    if (
        chunk.invisible_block_count == chunk::blocks_count ||
        chunk.fully_opaque_block_count == chunk::blocks_count
    ) {
        chunk.disp_list.clear();
        return mesh_update_state::should_continue;
    }

    size_t num_quads_written = generate_block_meshes_into_building_mesh((const block_type_t*)chunk.blocks.data());
    write_building_mesh_into_display_list(num_quads_written, &chunk.disp_list);
    
    // fprintf(log_file, "%p, %p, %p, %p\n", &chunk, building_quads, chunk.blocks.data(), chunk.disp_list.data());

    return mesh_update_state::should_break;
}

mesh_update_state game::update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk) {
    return mesh_update_state::should_break;
}