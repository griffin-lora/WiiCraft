#include "chunk_mesh_generation.hpp"
#include "block_core.hpp"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "block_functionality.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation.inl"
#include "log.hpp"
#include "block_new.hpp"
#include "pool.hpp"
#include "gfx/instruction_size.hpp"
#include "util.h"
#include <string.h>
#include <ogc/cache.h>

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

typedef struct {
    u8 px;
    u8 py;
    u8 pz;
    u8 txy;
} block_vertex_t;

typedef struct {
    block_vertex_t verts[4];
} block_quad_t;
static_assert(sizeof(block_quad_t) == 4 * 4, "");

#define NUM_BUILDING_QUADS 202

typedef block_quad_t building_quads_t[NUM_BUILDING_QUADS];
// static_assert(sizeof(building_quads_t) == 4096, "");

// typedef block_type_t block_chunk_t[16 * 16 * 16];
// static_assert(sizeof(block_chunk_t) == 4096, "");

typedef struct {
    alignas(32) building_quads_t solid;
    alignas(32) building_quads_t transparent;
    alignas(32) building_quads_t transparent_double_sided;
} building_quads_arrays_t;

static_assert(sizeof(building_quads_arrays_t) <= 4096*3, "");

static building_quads_arrays_t building_quads_arrays;

using namespace game;

typedef struct {
    size_t solid;
    size_t transparent;
    size_t transparent_double_sided;
} quads_indices_t;

static pool_display_list_t write_quads_into_display_list(size_t num_quads, const block_quad_t quads[]) {
    size_t num_verts = num_quads * 4;

    pool_display_list_t disp_list = {
        .size = align_to_32(
            get_begin_instruction_size(num_verts) +
            get_vector_instruction_size<u8>(3, num_verts) + 
            get_vector_instruction_size<u8>(2, num_verts)
        ),
        .chunk_index = acquire_pool_chunk()
    };
    disp_list.chunk = &pool_chunks[disp_list.chunk_index];
    memset(disp_list.chunk, 0, disp_list.size);
    DCInvalidateRange(disp_list.chunk, disp_list.size);

    GX_BeginDispList(disp_list.chunk, disp_list.size);
    GX_Begin(GX_QUADS, GX_VTXFMT0, num_verts);

    for (size_t i = 0; i < num_quads; i++) {
        const block_quad_t* quad = &quads[i];
        
        GX_Position3u8(quad->verts[0].px, quad->verts[0].py, quad->verts[0].pz);
        u8 tx = quad->verts[0].txy & ~0b10000000;
        u8 ty = (quad->verts[0].txy & 0b10000000) / 8;
        GX_TexCoord2u8(tx, ty);

        GX_Position3u8(quad->verts[1].px, quad->verts[1].py, quad->verts[1].pz);
        tx = quad->verts[1].txy & ~0b10000000;
        ty = (quad->verts[1].txy & 0b10000000) / 8;
        GX_TexCoord2u8(tx, ty);

        GX_Position3u8(quad->verts[2].px, quad->verts[2].py, quad->verts[2].pz);
        tx = quad->verts[2].txy & ~0b10000000;
        ty = (quad->verts[2].txy & 0b10000000) / 8;
        GX_TexCoord2u8(tx, ty);

        GX_Position3u8(quad->verts[3].px, quad->verts[3].py, quad->verts[3].pz);
        tx = quad->verts[3].txy & ~0b10000000;
        ty = (quad->verts[3].txy & 0b10000000) / 8;
        GX_TexCoord2u8(tx, ty);
    }
    
    GX_End();
    disp_list.size = GX_EndDispList(); // idk why i set the size

    return disp_list;
}

static void write_into_display_lists(std::vector<pool_display_list_t>* solid_display_lists, std::vector<pool_display_list_t>* transparent_display_lists, std::vector<pool_display_list_t>* transparent_double_sided_lists, quads_indices_t indices) {
    solid_display_lists->push_back(write_quads_into_display_list(indices.solid, building_quads_arrays.solid));
    transparent_display_lists->push_back(write_quads_into_display_list(indices.transparent, building_quads_arrays.transparent));
    transparent_double_sided_lists->push_back(write_quads_into_display_list(indices.transparent_double_sided, building_quads_arrays.transparent_double_sided));
}

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
        case block_type_debug:
        case block_type_grass:
        case block_type_stone:
        case block_type_dirt:
        case block_type_sand:
        case block_type_wood_planks:
        case block_type_stone_slab_both:
            return block_mesh_category_cube;
        case block_type_water:
            return block_mesh_category_transparent_cube;
        case block_type_tall_grass:
            return block_mesh_category_cross;
        case block_type_stone_slab_bottom:
            return block_mesh_category_slab_bottom;
        case block_type_stone_slab_top:
            return block_mesh_category_slab_top;
    }
}

static u8 get_face_tex(block_type_t type, block_face_t face) {
    switch (type) {
        case block_type_debug: return 0;
        case block_type_grass: switch (face) {
            default: return 4;
            case block_face_top: return 0;
            case block_face_bottom: return 2;
        }
        case block_type_stone: return 1;
        case block_type_dirt: return 2;
        case block_type_sand: return 6;
        case block_type_wood_planks: return 10;
        case block_type_stone_slab_bottom:
        case block_type_stone_slab_top:
        case block_type_stone_slab_both: switch (face) {
            default: return 8;
            case block_face_top: return 9;
        }
        case block_type_water: return 7;
    }
}

static u8 get_tex(block_type_t type) {
    return 5;
}

#define NUM_BLOCKS (16 * 16 * 16)
#define Z_OFFSET (16 * 16)
#define Y_OFFSET 16
#define X_OFFSET 1

typedef block_quad_t (*get_face_quad_function_t)(u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy);

static bool is_out_of_bounds(u32 x, u32 y, block_face_t face, size_t neighbor_index) {
    switch (face) {
        case block_face_front: if (x == 15) { return true; } break;
        case block_face_back: if (x == 0) { return true; } break;
        case block_face_top: if (y == 15) { return true; } break;
        case block_face_bottom: if (y == 0) { return true; } break;
        case block_face_left:
        case block_face_right: if (neighbor_index >= NUM_BLOCKS) { return true; } break;
    }
    return false;
}

static size_t get_neighbor_blocks_index(block_face_t face, size_t neighbor_index) {
    switch (face) {
        case block_face_front: return neighbor_index - Y_OFFSET;
        case block_face_back: return neighbor_index + Y_OFFSET;
        case block_face_top: return neighbor_index - Z_OFFSET;
        case block_face_bottom: return neighbor_index + Z_OFFSET;
        case block_face_right: return neighbor_index - NUM_BLOCKS;
        case block_face_left: return neighbor_index + NUM_BLOCKS;
    }
}

typedef struct {
    size_t solid;
    size_t transparent;
} face_quads_indices_t;

static face_quads_indices_t add_face_quad_if_needed(
    face_quads_indices_t quads_indices, const block_type_t block_types[], const block_type_t neighbor_block_types[], u32 x, u32 y, u32 z, block_type_t type, block_mesh_category_t category, block_face_t face, size_t neighbor_index, get_face_quad_function_t get_face_quad_function
) {
    block_type_t neighbor_block_type;
    if (is_out_of_bounds(x, y, face, neighbor_index)) {
        if (neighbor_block_types == NULL) {
            return quads_indices;
        }
        neighbor_block_type = neighbor_block_types[get_neighbor_blocks_index(face, neighbor_index)];
    } else {
        neighbor_block_type = block_types[neighbor_index];
    }
    block_mesh_category_t neighbor_mesh_category = get_block_mesh_category(neighbor_block_type);
    switch (category) {
        case block_mesh_category_cube: if (neighbor_mesh_category == block_mesh_category_cube) { return quads_indices; } break;
        case block_mesh_category_transparent_cube: if (neighbor_mesh_category == block_mesh_category_cube || neighbor_mesh_category == block_mesh_category_transparent_cube) { return quads_indices; } break;
        case block_mesh_category_slab_bottom: break;
        case block_mesh_category_slab_top: break;
    }
    u8 px = x * 4;
    u8 py = y * 4;
    u8 pz = z * 4;
    u8 pox = px + 4;
    u8 poy = py + 4;
    u8 poz = pz + 4;
    u8 txy = get_face_tex(type, face);
    u8 toxy = txy + 1;
    u8 txoy = txy | 0b10000000;
    u8 toxoy = toxy | 0b10000000;
    
    block_quad_t face_quad = get_face_quad_function(px, py, pz, pox, poy, poz, txy, toxy, txoy, toxoy);
    switch (category) {
        case block_mesh_category_cube:
        case block_mesh_category_slab_bottom:
        case block_mesh_category_slab_top: building_quads_arrays.solid[quads_indices.solid++] = face_quad; break;
        case block_mesh_category_transparent_cube: building_quads_arrays.transparent[quads_indices.transparent++] = face_quad; break;
    }

    return quads_indices;
}

static void generate_block_meshes(
    std::vector<pool_display_list_t>* solid_display_lists,
    std::vector<pool_display_list_t>* transparent_display_lists,
    std::vector<pool_display_list_t>* transparent_double_sided_display_lists,
    const block_type_t block_types[],
    const block_type_t front_block_types[],
    const block_type_t back_block_types[],
    const block_type_t top_block_types[],
    const block_type_t bottom_block_types[],
    const block_type_t right_block_types[],
    const block_type_t left_block_types[]
) {
    union {
        quads_indices_t all;
        face_quads_indices_t face;
    } quads_indices = {
        .all = {
            .solid = 0,
            .transparent = 0,
            .transparent_double_sided = 0
        }
    };

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
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, front_block_types, x, y, z, type, category, block_face_front, blocks_index + X_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { pox, poy, pz, txy },
                                { pox, py, pz, txoy },
                                { pox, py,poz, toxoy },
                                { pox, poy,poz, toxy }
                            }};
                        });
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, back_block_types, x, y, z, type, category, block_face_back, blocks_index - X_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { px, poy, pz, txy },	// Top Left of the quad (top)
                                { px, poy, poz, toxy },	// Top Right of the quad (top)
                                { px, py, poz, toxoy },	// Bottom Right of the quad (top)
                                { px, py, pz, txoy }		// Bottom Left of the quad (top)
                            }};
                        });
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, top_block_types, x, y, z, type, category, block_face_top, blocks_index + Y_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { px, poy, poz, txy },	// Bottom Left Of The Quad (Back)
                                { px, poy, pz, toxy },	// Bottom Right Of The Quad (Back)
                                { pox, poy, pz, toxoy },	// Top Right Of The Quad (Back)
                                { pox, poy, poz, txoy }	// Top Left Of The Quad (Back)
                            }};
                        });
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, bottom_block_types, x, y, z, type, category, block_face_bottom, blocks_index - Y_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { px, py, poz, txy },		// Top Right Of The Quad (Front)
                                { pox, py, poz, toxy },	// Top Left Of The Quad (Front)
                                { pox, py, pz, toxoy },	// Bottom Left Of The Quad (Front)
                                { px, py, pz, txoy }	// Bottom Right Of The Quad (Front)
                            }};
                        });
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, right_block_types, x, y, z, type, category, block_face_right, blocks_index + Z_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { pox, py, poz, txoy },	// Top Right Of The Quad (Right)
                                { px, py, poz, toxoy },		// Top Left Of The Quad (Right)
                                { px, poy, poz, toxy },	// Bottom Left Of The Quad (Right)
                                { pox, poy, poz, txy }	// Bottom Right Of The Quad (Right)
                            }};
                        });
                        quads_indices.face = add_face_quad_if_needed(quads_indices.face, block_types, left_block_types, x, y, z, type, category, block_face_left, blocks_index - Z_OFFSET, [](u8 px, u8 py, u8 pz, u8 pox, u8 poy, u8 poz, u8 txy, u8 toxy, u8 txoy, u8 toxoy) {
                            return block_quad_t{{
                                { pox, py, pz, txoy },	// Top Right Of The Quad (Left)
                                { pox, poy, pz, txy },	// Top Left Of The Quad (Left)
                                { px, poy, pz, toxy },	// Bottom Left Of The Quad (Left)
                                { px, py, pz, toxoy }	// Bottom Right Of The Quad (Left)
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
                        u8 txy = get_tex(type);
                        u8 toxy = txy + 1;
                        u8 txoy = txy | 0b10000000;
                        u8 toxoy = toxy | 0b10000000;
                        building_quads_arrays.transparent_double_sided[quads_indices.all.transparent_double_sided++] = block_quad_t{{
                            { px, py, pz, txoy },
                            { pox, py, poz, toxoy },
                            { pox, poy, poz, toxy },
                            { px, poy, pz, txy }
                        }};
                        building_quads_arrays.transparent_double_sided[quads_indices.all.transparent_double_sided++] = block_quad_t{{
                            { pox, py, pz, txoy },
                            { px, py, poz, toxoy },
                            { px, poy, poz, toxy },
                            { pox, poy, pz, txy }
                        }};
                    } break;
                }

                if (
                    quads_indices.all.solid >= (NUM_BUILDING_QUADS - 6) ||
                    quads_indices.all.transparent >= (NUM_BUILDING_QUADS - 6) ||
                    quads_indices.all.transparent_double_sided >= (NUM_BUILDING_QUADS - 1)
                ) [[unlikely]] {
                    write_into_display_lists(solid_display_lists, transparent_display_lists, transparent_double_sided_display_lists, quads_indices.all);
                    quads_indices.all = {
                        .solid = 0,
                        .transparent = 0,
                        .transparent_double_sided = 0
                    };
                }

                blocks_index++;
            }
        }
    }

    write_into_display_lists(solid_display_lists, transparent_display_lists, transparent_double_sided_display_lists, quads_indices.all);
    quads_indices.all = {
        .solid = 0,
        .transparent = 0,
        .transparent_double_sided = 0
    };
}

mesh_update_state game::update_core_mesh(chunk_quad_building_arrays& _, chunk& chunk) {
    for (pool_display_list_t disp_list : chunk.solid_display_lists) {
        release_pool_chunk(disp_list.chunk_index);
    }
    for (pool_display_list_t disp_list : chunk.transparent_display_lists) {
        release_pool_chunk(disp_list.chunk_index);
    }
    for (pool_display_list_t disp_list : chunk.transparent_double_sided_display_lists) {
        release_pool_chunk(disp_list.chunk_index);
    }

    chunk.solid_display_lists.clear();
    chunk.transparent_display_lists.clear();
    chunk.transparent_double_sided_display_lists.clear();
    if (
        chunk.invisible_block_count == chunk::blocks_count ||
        chunk.fully_opaque_block_count == chunk::blocks_count
    ) {
        return mesh_update_state::should_continue;
    }

    generate_block_meshes(
        &chunk.solid_display_lists,
        &chunk.transparent_display_lists,
        &chunk.transparent_double_sided_display_lists,
        (const block_type_t*)chunk.blocks,
        chunk.nh.front.has_value() ? (const block_type_t*)chunk.nh.front->get().blocks : NULL,
        chunk.nh.back.has_value() ? (const block_type_t*)chunk.nh.back->get().blocks : NULL,
        chunk.nh.top.has_value() ? (const block_type_t*)chunk.nh.top->get().blocks : NULL,
        chunk.nh.bottom.has_value() ? (const block_type_t*)chunk.nh.bottom->get().blocks : NULL,
        chunk.nh.right.has_value() ? (const block_type_t*)chunk.nh.right->get().blocks : NULL,
        chunk.nh.left.has_value() ? (const block_type_t*)chunk.nh.left->get().blocks : NULL
    );

    return mesh_update_state::should_break;
}