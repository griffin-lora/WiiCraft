#include "block_world_mesh_generation.h"
#include "log.h"
#include "block.h"
#include "pool.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <string.h>
#include <ogc/cache.h>
#include <ogc/gx.h>

// Possible future optimization is to stop generating the mesh after we reach the end of blocks to generate meshes from

typedef struct {
    u8 type;
    u8 x;
    u8 y;
    u8 z;
} block_mesh_t;

_Static_assert(sizeof(block_mesh_t) == 4, "");

#define NUM_SOLID_BUILDING_MESHES 204
#define NUM_TRANSPARENT_BUILDING_MESHES 204
#define NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES 102

typedef struct {
    _Alignas(32) block_mesh_t solid[NUM_SOLID_BUILDING_MESHES];
    _Alignas(32) block_mesh_t transparent[NUM_TRANSPARENT_BUILDING_MESHES];
    _Alignas(32) block_mesh_t transparent_double_sided[NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES];
} building_meshes_arrays_t;

_Static_assert(sizeof(building_meshes_arrays_t) <= 4096*3, "");

static building_meshes_arrays_t building_meshes_arrays;

typedef struct {
    size_t solid;
    size_t transparent;
    size_t transparent_double_sided;
} meshes_indices_t;



typedef enum __attribute__((__packed__)) {
    block_mesh_category_invisible,
    block_mesh_category_cube,
    block_mesh_category_transparent_cube,
    block_mesh_category_cross
} block_mesh_category_t;

_Static_assert(sizeof(block_mesh_category_t) == 1);

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

static u16 write_meshes_into_display_list(block_display_list_type_t type, vec3s world_pos, size_t num_meshes, const block_mesh_t meshes[]) {
    size_t num_verts = num_meshes * ((type != block_display_list_type_transparent_double_sided) ? 4 : 8);

    block_display_list_t* disp_list = acquire_block_display_list_pool_chunk(type);
    disp_list->x = world_pos.x;
    disp_list->y = world_pos.y;
    disp_list->z = world_pos.z;
    u16 chunk_index = disp_list->chunk_index;
    u16 display_list_size = (u16)align_to_32(
        GET_BEGIN_INSTRUCTION_SIZE(num_verts) +
        GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(u8), num_verts) + 
        GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), num_verts)
    );

    block_display_list_chunk_t* chunks = get_block_display_list_pool(type)->chunks;
    void* chunk = &chunks[chunk_index];
    memset(chunk, 0, display_list_size);
    DCInvalidateRange(chunk, display_list_size);

    GX_BeginDispList(chunk, display_list_size);
    GX_Begin(GX_QUADS, GX_VTXFMT0, num_verts);

    switch (type) {
        case block_display_list_type_solid:
        case block_display_list_type_transparent:
            for (size_t i = 0; i < num_meshes; i++) {
                block_mesh_t mesh = meshes[i];

                block_type_t block_type = (block_type_t)(mesh.type / 8);
                block_face_t block_face = (block_face_t)(mesh.type % 8);
                u8 px = mesh.x * 4;
                u8 py = mesh.y * 4;
                u8 pz = mesh.z * 4;
                u8 pox = px + 4;
                u8 poy = py + 4;
                u8 poz = pz + 4;
                u8 tx = get_face_tex(block_type, block_face);
                u8 tox = tx + 1;
                u8 ty = 0;
                u8 toy = 16;

                switch (block_face) {
                    case block_face_front:
                        GX_Position3u8(pox, poy, pz);
                        GX_TexCoord2u8(tx, ty);
                        GX_Position3u8(pox, py, pz);
                        GX_TexCoord2u8(tx, toy);
                        GX_Position3u8(pox, py,poz);
                        GX_TexCoord2u8(tox, toy);
                        GX_Position3u8(pox, poy,poz);
                        GX_TexCoord2u8(tox, ty);
                        break;
                    case block_face_back:
                        GX_Position3u8(px, poy, pz);
                        GX_TexCoord2u8(tx, ty);	// Top Left of the quad (top)
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tox, ty);	// Top Right of the quad (top)
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tox, toy);	// Bottom Right of the quad (top)
                        GX_Position3u8(px, py, pz);
                        GX_TexCoord2u8(tx, toy);		// Bottom Left of the quad (top)
                        break;
                    case block_face_top:
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tx, ty);	// Bottom Left Of The Quad (Back)
                        GX_Position3u8(px, poy, pz);
                        GX_TexCoord2u8(tox, ty);	// Bottom Right Of The Quad (Back)
                        GX_Position3u8(pox, poy, pz); 
                        GX_TexCoord2u8(tox, toy);	// Top Right Of The Quad (Back)
                        GX_Position3u8(pox, poy, poz);
                        GX_TexCoord2u8(tx, toy);	// Top Left Of The Quad (Back)
                        break;
                    case block_face_bottom:
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tx, ty);		// Top Right Of The Quad (Front)
                        GX_Position3u8(pox, py, poz);
                        GX_TexCoord2u8(tox, ty);	// Top Left Of The Quad (Front)
                        GX_Position3u8(pox, py, pz);
                        GX_TexCoord2u8(tox, toy);	// Bottom Left Of The Quad (Front)
                        GX_Position3u8(px, py, pz); 
                        GX_TexCoord2u8(tx, toy);	// Bottom Right Of The Quad (Front)
                        break;
                    case block_face_right:
                        GX_Position3u8(pox, py, poz);
                        GX_TexCoord2u8(tx, toy);	// Top Right Of The Quad (Right)
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tox, toy);		// Top Left Of The Quad (Right)
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tox, ty);	// Bottom Left Of The Quad (Right)
                        GX_Position3u8(pox, poy, poz);
                        GX_TexCoord2u8(tx, ty);	// Bottom Right Of The Quad (Right)
                        break;
                    case block_face_left:
                        GX_Position3u8(pox, py, pz);
                        GX_TexCoord2u8(tx, toy);	// Top Right Of The Quad (Left)
                        GX_Position3u8(pox, poy, pz); 
                        GX_TexCoord2u8(tx, ty);	// Top Left Of The Quad (Left)
                        GX_Position3u8(px, poy, pz);
                        GX_TexCoord2u8(tox, ty);	// Bottom Left Of The Quad (Left)
                        GX_Position3u8(px, py, pz);
                        GX_TexCoord2u8(tox, toy);	// Bottom Right Of The Quad (Left)
                        break;
                }
            }
            break;
        case block_display_list_type_transparent_double_sided:
            for (size_t i = 0; i < num_meshes; i++) {
                block_mesh_t mesh = meshes[i];

                u8 px = mesh.x * 4;
                u8 py = mesh.y * 4;
                u8 pz = mesh.z * 4;
                u8 pox = px + 4;
                u8 poy = py + 4;
                u8 poz = pz + 4;
                u8 tx = get_tex((block_type_t)mesh.type);
                u8 tox = tx + 1;
                u8 ty = 0;
                u8 toy = 16;

                GX_Position3u8(px, py, pz);
                GX_TexCoord2u8(tx, toy);
                GX_Position3u8(pox, py, poz);
                GX_TexCoord2u8(tox, toy);
                GX_Position3u8(pox, poy, poz);
                GX_TexCoord2u8(tox, ty);
                GX_Position3u8(px, poy, pz);
                GX_TexCoord2u8(tx, ty);

                GX_Position3u8(pox, py, pz);
                GX_TexCoord2u8(tx, toy);
                GX_Position3u8(px, py, poz);
                GX_TexCoord2u8(tox, toy);
                GX_Position3u8(px, poy, poz);
                GX_TexCoord2u8(tox, ty);
                GX_Position3u8(pox, poy, pz);
                GX_TexCoord2u8(tx, ty);
            }
            break;
    }
    
    GX_End();
    disp_list->size = GX_EndDispList();

    return chunk_index;
}

#define NUM_BLOCKS NUM_BLOCKS_PER_BLOCK_CHUNK
#define Z_OFFSET BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET
#define Y_OFFSET BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET
#define X_OFFSET BLOCKS_PER_BLOCK_CHUNK_X_OFFSET

#define NUM_ROW_BLOCKS NUM_ROW_BLOCKS_PER_BLOCK_CHUNK

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
} face_meshes_indices_t;

static face_meshes_indices_t add_face_mesh_if_needed(
    face_meshes_indices_t indices, const block_type_t block_types[], const block_type_t neighbor_block_types[], u32 x, u32 y, u32 z, block_type_t type, block_mesh_category_t category, block_face_t face, size_t neighbor_index
) {
    block_type_t neighbor_block_type;
    if (is_out_of_bounds(x, y, face, neighbor_index)) {
        if (neighbor_block_types == NULL) {
            return indices;
        }
        neighbor_block_type = neighbor_block_types[get_neighbor_blocks_index(face, neighbor_index)];
    } else {
        neighbor_block_type = block_types[neighbor_index];
    }
    block_mesh_category_t neighbor_mesh_category = get_block_mesh_category(neighbor_block_type);
    switch (category) {
        case block_mesh_category_cube: if (neighbor_mesh_category == block_mesh_category_cube) { return indices; } break;
        case block_mesh_category_transparent_cube: if (neighbor_mesh_category == block_mesh_category_cube || neighbor_mesh_category == block_mesh_category_transparent_cube) { return indices; } break;
    }
    switch (category) {
        case block_mesh_category_cube:
            building_meshes_arrays.solid[indices.solid++] = (block_mesh_t){
                .type = (u8)((type * 8) + face),
                .x = (u8)x,
                .y = (u8)y,
                .z = (u8)z
            };
            break;
        case block_mesh_category_transparent_cube:
            building_meshes_arrays.transparent[indices.transparent++] = (block_mesh_t){
                .type = (u8)((type * 8) + face),
                .x = (u8)x,
                .y = (u8)y,
                .z = (u8)z
            };
            break;
    }

    return indices;
}

void update_block_chunk_visuals(
    vec3s world_pos,
    block_display_list_chunk_descriptor_t descriptors[],
    const block_type_t block_types[],
    const block_type_t front_block_types[],
    const block_type_t back_block_types[],
    const block_type_t top_block_types[],
    const block_type_t bottom_block_types[],
    const block_type_t right_block_types[],
    const block_type_t left_block_types[]
) {
    for (size_t i = 0; descriptors[i].type != 0xff; i++) {
        release_block_display_list_pool_chunk(descriptors[i].type, descriptors[i].chunk_index);
    }

    // if (
    //     chunk.invisible_block_count == chunk::blocks_count ||
    //     chunk.fully_opaque_block_count == chunk::blocks_count
    // ) {
    //     return mesh_update_state::should_continue;
    // }

    union {
        meshes_indices_t all;
        face_meshes_indices_t face;
    } indices = {
        .all = {
            .solid = 0,
            .transparent = 0,
            .transparent_double_sided = 0
        }
    };

    size_t descriptor_index = 0;

    // Generate mesh for faces that are not neighboring another chunk.
    size_t blocks_index = 0;
    for (u32 z = 0; z < NUM_ROW_BLOCKS; z++) {
        for (u32 y = 0; y < NUM_ROW_BLOCKS; y++) {
            for (u32 x = 0; x < NUM_ROW_BLOCKS; x++) {
                block_type_t type = block_types[blocks_index];

                block_mesh_category_t category = get_block_mesh_category(type);
                switch (category) {
                    default: break;
                    case block_mesh_category_cube:
                    case block_mesh_category_transparent_cube: {
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, front_block_types, x, y, z, type, category, block_face_front, blocks_index + X_OFFSET);
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, back_block_types, x, y, z, type, category, block_face_back, blocks_index - X_OFFSET);
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, top_block_types, x, y, z, type, category, block_face_top, blocks_index + Y_OFFSET);
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, bottom_block_types, x, y, z, type, category, block_face_bottom, blocks_index - Y_OFFSET);
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, right_block_types, x, y, z, type, category, block_face_right, blocks_index + Z_OFFSET);
                        indices.face = add_face_mesh_if_needed(indices.face, block_types, left_block_types, x, y, z, type, category, block_face_left, blocks_index - Z_OFFSET);
                    } break;
                    case block_mesh_category_cross: {
                        building_meshes_arrays.transparent_double_sided[indices.all.transparent_double_sided++] = (block_mesh_t){
                            .type = (u8)type,
                            .x = (u8)x,
                            .y = (u8)y,
                            .z = (u8)z
                        };
                    } break;
                }

                if (indices.all.solid >= (NUM_SOLID_BUILDING_MESHES - 6)) {
                    descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
                        .type = block_display_list_type_solid,
                        .chunk_index = write_meshes_into_display_list(block_display_list_type_solid, world_pos, indices.all.solid, building_meshes_arrays.solid)
                    };
                    indices.all.solid = 0;
                }
                if (indices.all.transparent >= (NUM_TRANSPARENT_BUILDING_MESHES - 6)) {
                    descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
                        .type = block_display_list_type_transparent,
                        .chunk_index = write_meshes_into_display_list(block_display_list_type_transparent, world_pos, indices.all.transparent, building_meshes_arrays.transparent)
                    };
                    indices.all.transparent = 0;
                }
                if (indices.all.transparent_double_sided >= (NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES - 1)) {
                    descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
                        .type = block_display_list_type_transparent_double_sided,
                        .chunk_index = write_meshes_into_display_list(block_display_list_type_transparent_double_sided, world_pos, indices.all.transparent_double_sided, building_meshes_arrays.transparent_double_sided)
                    };
                    indices.all.transparent_double_sided = 0;
                }

                blocks_index++;
            }
        }
    }

    if (indices.all.solid > 0) {
        descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
            .type = block_display_list_type_solid,
            .chunk_index = write_meshes_into_display_list(block_display_list_type_solid, world_pos, indices.all.solid, building_meshes_arrays.solid)
        };
    }
    if (indices.all.transparent > 0) {
        descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
            .type = block_display_list_type_transparent,
            .chunk_index = write_meshes_into_display_list(block_display_list_type_transparent, world_pos, indices.all.transparent, building_meshes_arrays.transparent)
        };
    }
    if (indices.all.transparent_double_sided > 0) {
        descriptors[descriptor_index++] = (block_display_list_chunk_descriptor_t){
            .type = block_display_list_type_transparent_double_sided,
            .chunk_index = write_meshes_into_display_list(block_display_list_type_transparent_double_sided, world_pos, indices.all.transparent_double_sided, building_meshes_arrays.transparent_double_sided)
        };
    }
}