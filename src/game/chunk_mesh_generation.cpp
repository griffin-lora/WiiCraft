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

typedef enum : u8 {
    block_mesh_category_invisible,
    block_mesh_category_cube,
    block_mesh_category_transparent_cube,
    block_mesh_category_cross,
    block_mesh_category_slab_bottom,
    block_mesh_category_slab_top
} block_mesh_category_t;

static_assert(sizeof(block_mesh_category_t) == 1);

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

static u8 get_face_tex_x(block_type_t type, block_face_t face) {
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
        case block::type::stone_slab_bottom:
        case block::type::stone_slab_top:
        case block::type::stone_slab_both: switch (face) {
            default: return 32;
            case block::face::top: return 36;
        }
        case block::type::water: return 28;
    }
}

static u8 get_face_tex_y(block_type_t type, block_face_t face) {
    return 0;
}

static u8 get_tex_x(block_type_t type) {
    return 20;
}

static u8 get_tex_y(block_type_t type) {
    return 0;
}

#define NUM_BLOCKS (32 * 32 * 32)
#define Z_OFFSET (32 * 32)
#define Y_OFFSET 32
#define X_OFFSET 1

typedef chunk_mesh_quad_t (*get_face_quad_function_t)(u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy);

static bool is_out_of_bounds(u32 x, u32 y, block_face_t face, size_t neighbor_index) {
    switch (face) {
        case block::face::front: if (x == 31) { return true; } break;
        case block::face::back: if (x == 0) { return true; } break;
        case block::face::top: if (y == 31) { return true; } break;
        case block::face::bottom: if (y == 0) { return true; } break;
        case block::face::left:
        case block::face::right: if (neighbor_index >= NUM_BLOCKS) { return true; } break;
    }
    return false;
}

static size_t get_neighbor_blocks_index(block_face_t face, size_t neighbor_index) {
    switch (face) {
        case block::face::front: return neighbor_index - Y_OFFSET;
        case block::face::back: return neighbor_index + Y_OFFSET;
        case block::face::top: return neighbor_index - Z_OFFSET;
        case block::face::bottom: return neighbor_index + Z_OFFSET;
        case block::face::right: return neighbor_index - NUM_BLOCKS;
        case block::face::left: return neighbor_index + NUM_BLOCKS;
    }
}

static size_t add_face_mesh_if_needed(size_t quads_index, const block_type_t block_types[], const block_type_t neighbor_block_types[], u32 x, u32 y, u32 z, block_type_t type, block_mesh_category_t category, block_face_t face, size_t neighbor_index, get_face_quad_function_t get_face_quad_function) {
    block_type_t neighbor_block_type;
    if (is_out_of_bounds(x, y, face, neighbor_index)) {
        if (neighbor_block_types == NULL) {
            return quads_index;
        }
        neighbor_block_type = neighbor_block_types[get_neighbor_blocks_index(face, neighbor_index)];
    } else {
        neighbor_block_type = block_types[neighbor_index];
    }
    block_mesh_category_t neighbor_mesh_category = get_block_mesh_category(neighbor_block_type);
    switch (category) {
        case block_mesh_category_cube: if (neighbor_mesh_category == block_mesh_category_cube) { return quads_index; } break;
        case block_mesh_category_transparent_cube: if (neighbor_mesh_category == block_mesh_category_cube || neighbor_mesh_category == block_mesh_category_transparent_cube) { return quads_index; } break;
        default: break;
    }
    u8 px = x * 4;
    u8 py = y * 4;
    u8 pz = z * 4;
    u8 pox = px + 4;
    u8 poy = py + 4;
    u8 poz = pz + 4;
    u8 tx = get_face_tex_x(type, face);
    u8 ty = get_face_tex_y(type, face);
    u8 tox = tx + 4;
    u8 toy = ty + 4;
    
    building_quads[quads_index++] = get_face_quad_function(px, py, pz, pox, poy, poz, tx, ty, tox, toy);

    return quads_index;
}

static size_t generate_block_meshes_into_building_mesh(
    const block_type_t block_types[],
    const block_type_t front_block_types[],
    const block_type_t back_block_types[],
    const block_type_t top_block_types[],
    const block_type_t bottom_block_types[],
    const block_type_t right_block_types[],
    const block_type_t left_block_types[]
) {
    size_t quads_index = 0;

    // Generate mesh for faces that are not neighboring another chunk.
    size_t blocks_index = 0;
    for (u32 z = 0; z < chunk::size; z++) {
        for (u32 y = 0; y < chunk::size; y++) {
            for (u32 x = 0; x < chunk::size; x++) {
                block_type_t type = block_types[blocks_index];

                block_mesh_category_t category = get_block_mesh_category(type);
                switch (category) {
                    default: break;
                    case block_mesh_category_cube:
                    case block_mesh_category_transparent_cube:
                    case block_mesh_category_slab_bottom:
                    case block_mesh_category_slab_top: {
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, front_block_types, x, y, z, type, category, block::face::front, blocks_index + X_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                { pox,poy, pz, tx,ty },
                                { pox,py, pz, tx, toy },
                                { pox,py,poz, tox,toy },
                                { pox,poy,poz, tox, ty }
                            }};
                        });
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, back_block_types, x, y, z, type, category, block::face::back, blocks_index - X_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                {  px, poy, pz, tx,ty },	// Top Left of the quad (top)
                                { px, poy, poz, tox,ty },	// Top Right of the quad (top)
                                { px, py, poz, tox,toy },	// Bottom Right of the quad (top)
                                { px, py, pz, tx,toy }		// Bottom Left of the quad (top)
                            }};
                        });
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, top_block_types, x, y, z, type, category, block::face::top, blocks_index + Y_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                { px, poy, poz, tx, ty },	// Bottom Left Of The Quad (Back)
                                { px, poy, pz, tox, ty },	// Bottom Right Of The Quad (Back)
                                { pox, poy, pz, tox, toy },	// Top Right Of The Quad (Back)
                                { pox, poy, poz, tx, toy }	// Top Left Of The Quad (Back)
                            }};
                        });
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, bottom_block_types, x, y, z, type, category, block::face::bottom, blocks_index - Y_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                { px, py, poz, tx, ty },		// Top Right Of The Quad (Front)
                                { pox, py, poz, tox, ty },	// Top Left Of The Quad (Front)
                                { pox, py, pz, tox, toy },	// Bottom Left Of The Quad (Front)
                                { px, py, pz, tx, toy }	// Bottom Right Of The Quad (Front)
                            }};
                        });
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, right_block_types, x, y, z, type, category, block::face::right, blocks_index + Z_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                { pox, py,poz, tx,toy },	// Top Right Of The Quad (Right)
                                { px, py, poz, tox,toy },		// Top Left Of The Quad (Right)
                                { px,poy, poz, tox,ty },	// Bottom Left Of The Quad (Right)
                                { pox,poy,poz, tx,ty }	// Bottom Right Of The Quad (Right)
                            }};
                        });
                        quads_index = add_face_mesh_if_needed(quads_index, block_types, left_block_types, x, y, z, type, category, block::face::left, blocks_index - Z_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 tx, u8 ty, u8 tox, u8 toy) {
                            return chunk_mesh_quad_t{{
                                { pox, py, pz, tx,toy },	// Top Right Of The Quad (Left)
                                { pox, poy,pz, tx,ty },	// Top Left Of The Quad (Left)
                                { px,poy,pz, tox,ty },	// Bottom Left Of The Quad (Left)
                                { px,py, pz, tox,toy }	// Bottom Right Of The Quad (Left)
                            }};
                        });
                    } break;
                    case block_mesh_category_cross: {
                        u8 px = x * 4;
                        u8 py = y * 4;
                        u8 pz = z * 4;
                        u8 pox = px + 4;
                        u8 poy = py + 4;
                        u8 poz = pz + 4;
                        u8 tx = get_tex_x(type);
                        u8 ty = get_tex_y(type);
                        u8 tox = tx + 4;
                        u8 toy = ty + 4;
                        building_quads[quads_index++] = chunk_mesh_quad_t{{
                            { px, py, pz, tx, toy },
                            { pox, py, poz, tox, toy },
                            { pox, poy, poz, tox, ty },
                            { px, poy, pz, tx, ty }
                        }};
                        building_quads[quads_index++] = chunk_mesh_quad_t{{
                            { pox, py, pz, tx, toy },
                            { px, py, poz, tox, toy },
                            { px, poy, poz, tox, ty },
                            { pox, poy, pz, tx, ty }
                        }};
                    } break;
                }

                if (quads_index >= NUM_BUILDING_QUADS) [[unlikely]] {
                    lprintf("Too many quads for should be %d, count is %d\n", NUM_BUILDING_QUADS, quads_index);
                    return quads_index;
                }

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

    size_t num_quads_written = generate_block_meshes_into_building_mesh(
        (const block_type_t*)chunk.blocks.data(),
        chunk.nh.front.has_value() ? (const block_type_t*)chunk.nh.front->get().blocks.data() : NULL,
        chunk.nh.back.has_value() ? (const block_type_t*)chunk.nh.back->get().blocks.data() : NULL,
        chunk.nh.top.has_value() ? (const block_type_t*)chunk.nh.top->get().blocks.data() : NULL,
        chunk.nh.bottom.has_value() ? (const block_type_t*)chunk.nh.bottom->get().blocks.data() : NULL,
        chunk.nh.right.has_value() ? (const block_type_t*)chunk.nh.right->get().blocks.data() : NULL,
        chunk.nh.left.has_value() ? (const block_type_t*)chunk.nh.left->get().blocks.data() : NULL
    );
    write_building_mesh_into_display_list(num_quads_written, &chunk.disp_list);
    
    // u8 stack_var;
    // lprintf("%p, %p, %p, %p\n", &stack_var, building_quads, chunk.blocks.data(), chunk.disp_list.data());

    return mesh_update_state::should_break;
}