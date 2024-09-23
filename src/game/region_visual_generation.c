#include "region_visual_generation.h"
#include "chunk.h"
#include "game/display_list.h"
#include "game/region.h"
#include "game/region_rendering.h"
#include "log.h"
#include "voxel.h"
#include "gfx/instruction_size.h"
#include "util.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/cache.h>
#include <ogc/gx.h>

// Possible future optimization is to stop generating the mesh after we reach the end of voxels to generate meshes from

typedef struct {
    u8 type;
    u8 x;
    u8 y;
    u8 z;
} voxel_mesh_t;

static_assert(sizeof(voxel_mesh_t) == 4, "");

#define NUM_SOLID_BUILDING_MESHES 204
#define NUM_TRANSPARENT_BUILDING_MESHES 204
#define NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES 102

typedef struct {
    alignas(32) voxel_mesh_t solid[NUM_SOLID_BUILDING_MESHES];
    alignas(32) voxel_mesh_t transparent[NUM_TRANSPARENT_BUILDING_MESHES];
    alignas(32) voxel_mesh_t transparent_double_sided[NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES];
} building_meshes_arrays_t;

static_assert(sizeof(building_meshes_arrays_t) <= 4096*3, "");

static building_meshes_arrays_t building_meshes_arrays;

typedef struct {
    size_t solid;
    size_t transparent;
    size_t transparent_double_sided;
} meshes_indices_t;

typedef enum __attribute__((__packed__)) {
    voxel_mesh_category_invisible,
    voxel_mesh_category_cube,
    voxel_mesh_category_transparent_cube,
    voxel_mesh_category_cross
} voxel_mesh_category_t;

static_assert(sizeof(voxel_mesh_category_t) == 1);

static voxel_mesh_category_t get_voxel_mesh_category(voxel_type_t type) {
    switch (type) {
        default:
            return voxel_mesh_category_invisible;
        case voxel_type_debug:
        case voxel_type_grass:
        case voxel_type_stone:
        case voxel_type_dirt:
        case voxel_type_sand:
        case voxel_type_wood_planks:
        case voxel_type_stone_slab_both:
            return voxel_mesh_category_cube;
        case voxel_type_water:
            return voxel_mesh_category_transparent_cube;
        case voxel_type_tall_grass:
            return voxel_mesh_category_cross;
    }
}

static u8 get_face_tex(voxel_type_t type, voxel_face_t face) {
    switch (type) {
        default: break;
        case voxel_type_debug: return 0;
        case voxel_type_grass: switch (face) {
            default: return 4;
            case voxel_face_top: return 0;
            case voxel_face_bottom: return 2;
        }
        case voxel_type_stone: return 1;
        case voxel_type_dirt: return 2;
        case voxel_type_sand: return 6;
        case voxel_type_wood_planks: return 10;
        case voxel_type_stone_slab_both: switch (face) {
            default: return 8;
            case voxel_face_top: return 9;
        }
        case voxel_type_water: return 7;
    }
    return 0;
}

static u8 get_tex(voxel_type_t) {
    return 5;
}

static void write_meshes_into_display_list(size_t display_list_array_index, size_t num_meshes, size_t num_verts, const voxel_mesh_t meshes[], region_render_info_t* render_info) {
    u16 num_display_list_bytes = (u16) align_to_32(
        BEGIN_INSTRUCTION_SIZE +
        GET_VECTOR_INSTRUCTION_SIZE(3, sizeof(u8), num_verts) + 
        GET_VECTOR_INSTRUCTION_SIZE(2, sizeof(u8), num_verts)
    );
    void* display_list_data = memalign(32, NUM_CHUNK_BYTES);

    memset(display_list_data, 0, num_display_list_bytes);
    DCInvalidateRange(display_list_data, num_display_list_bytes);

    GX_BeginDispList(display_list_data, num_display_list_bytes);
    GX_Begin(GX_QUADS, REGION_VERTEX_FORMAT_INDEX, (u16) num_verts);

    switch (display_list_array_index) {
        case 0:
        case 1:
            for (size_t i = 0; i < num_meshes; i++) {
                voxel_mesh_t mesh = meshes[i];

                voxel_type_t voxel_type = (voxel_type_t) (mesh.type / 8);
                voxel_face_t voxel_face = (voxel_face_t) (mesh.type % 8);
                u8 px = mesh.x * 4;
                u8 py = mesh.y * 4;
                u8 pz = mesh.z * 4;
                u8 pox = px + 4;
                u8 poy = py + 4;
                u8 poz = pz + 4;
                u8 tx = get_face_tex(voxel_type, voxel_face);
                u8 tox = tx + 1;
                u8 ty = 0;
                u8 toy = 16;

                switch (voxel_face) {
                    case voxel_face_front:
                        GX_Position3u8(pox, poy, pz);
                        GX_TexCoord2u8(tx, ty);
                        GX_Position3u8(pox, py, pz);
                        GX_TexCoord2u8(tx, toy);
                        GX_Position3u8(pox, py,poz);
                        GX_TexCoord2u8(tox, toy);
                        GX_Position3u8(pox, poy,poz);
                        GX_TexCoord2u8(tox, ty);
                        break;
                    case voxel_face_back:
                        GX_Position3u8(px, poy, pz);
                        GX_TexCoord2u8(tx, ty);	// Top Left of the quad (top)
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tox, ty);	// Top Right of the quad (top)
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tox, toy);	// Bottom Right of the quad (top)
                        GX_Position3u8(px, py, pz);
                        GX_TexCoord2u8(tx, toy);		// Bottom Left of the quad (top)
                        break;
                    case voxel_face_top:
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tx, ty);	// Bottom Left Of The Quad (Back)
                        GX_Position3u8(px, poy, pz);
                        GX_TexCoord2u8(tox, ty);	// Bottom Right Of The Quad (Back)
                        GX_Position3u8(pox, poy, pz); 
                        GX_TexCoord2u8(tox, toy);	// Top Right Of The Quad (Back)
                        GX_Position3u8(pox, poy, poz);
                        GX_TexCoord2u8(tx, toy);	// Top Left Of The Quad (Back)
                        break;
                    case voxel_face_bottom:
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tx, ty);		// Top Right Of The Quad (Front)
                        GX_Position3u8(pox, py, poz);
                        GX_TexCoord2u8(tox, ty);	// Top Left Of The Quad (Front)
                        GX_Position3u8(pox, py, pz);
                        GX_TexCoord2u8(tox, toy);	// Bottom Left Of The Quad (Front)
                        GX_Position3u8(px, py, pz); 
                        GX_TexCoord2u8(tx, toy);	// Bottom Right Of The Quad (Front)
                        break;
                    case voxel_face_right:
                        GX_Position3u8(pox, py, poz);
                        GX_TexCoord2u8(tx, toy);	// Top Right Of The Quad (Right)
                        GX_Position3u8(px, py, poz);
                        GX_TexCoord2u8(tox, toy);		// Top Left Of The Quad (Right)
                        GX_Position3u8(px, poy, poz);
                        GX_TexCoord2u8(tox, ty);	// Bottom Left Of The Quad (Right)
                        GX_Position3u8(pox, poy, poz);
                        GX_TexCoord2u8(tx, ty);	// Bottom Right Of The Quad (Right)
                        break;
                    case voxel_face_left:
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
        case 2:
            for (size_t i = 0; i < num_meshes; i++) {
                voxel_mesh_t mesh = meshes[i];

                u8 px = mesh.x * 4;
                u8 py = mesh.y * 4;
                u8 pz = mesh.z * 4;
                u8 pox = px + 4;
                u8 poy = py + 4;
                u8 poz = pz + 4;
                u8 tx = get_tex((voxel_type_t) mesh.type);
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
    num_display_list_bytes = (u16) GX_EndDispList();

    region_display_list_array_t* array = &render_info->display_list_arrays[display_list_array_index];
    array->num_display_lists++;
    array->display_lists = realloc(array->display_lists, array->num_display_lists * sizeof(*array->display_lists));

    array->display_lists[array->num_display_lists - 1] = (display_list_t) {
        .num_bytes = num_display_list_bytes,
        .data = display_list_data
    };
}

typedef struct {
    size_t solid;
    size_t transparent;
} face_meshes_indices_t;

static face_meshes_indices_t add_face_mesh_if_needed(
    face_meshes_indices_t indices, const voxel_type_array_t* voxel_types, const voxel_type_array_t* neighbor_voxel_types, u32 x, u32 y, u32 z, voxel_type_t type, voxel_mesh_category_t category, voxel_face_t face, u32 nx, u32 ny, u32 nz
) {
    voxel_type_t neighbor_voxel_type;
    if (nx >= (u32) REGION_SIZE || ny >= (u32) REGION_SIZE || nz >= (u32) REGION_SIZE) {
        if (neighbor_voxel_types == NULL) {
            return indices;
        }
        neighbor_voxel_type = neighbor_voxel_types->types[nx % (u32) REGION_SIZE][ny % (u32) REGION_SIZE][nz % (u32) REGION_SIZE];
    } else {
        neighbor_voxel_type = voxel_types->types[nx][ny][nz];
    }
    voxel_mesh_category_t neighbor_mesh_category = get_voxel_mesh_category(neighbor_voxel_type);
    switch (category) {
        default: break;
        case voxel_mesh_category_cube: if (neighbor_mesh_category == voxel_mesh_category_cube) { return indices; } break;
        case voxel_mesh_category_transparent_cube: if (neighbor_mesh_category == voxel_mesh_category_cube || neighbor_mesh_category == voxel_mesh_category_transparent_cube) { return indices; } break;
    }
    switch (category) {
        default: break;
        case voxel_mesh_category_cube:
            building_meshes_arrays.solid[indices.solid++] = (voxel_mesh_t) {
                .type = (u8) ((type * 8) + face),
                .x = (u8) x,
                .y = (u8) y,
                .z = (u8) z
            };
            break;
        case voxel_mesh_category_transparent_cube:
            building_meshes_arrays.transparent[indices.transparent++] = (voxel_mesh_t) {
                .type = (u8) ((type * 8) + face),
                .x = (u8) x,
                .y = (u8) y,
                .z = (u8) z
            };
            break;
    }

    return indices;
}

void generate_region_visuals(
    const voxel_type_array_t* voxel_types,
    const voxel_type_array_t* front_voxel_types,
    const voxel_type_array_t* back_voxel_types,
    const voxel_type_array_t* top_voxel_types,
    const voxel_type_array_t* bottom_voxel_types,
    const voxel_type_array_t* right_voxel_types,
    const voxel_type_array_t* left_voxel_types,
    region_render_info_t* render_info
) {
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

    // Generate mesh for faces that are not neighboring another region.
    for (u32 x = 0; x < REGION_SIZE; x++) {
        for (u32 y = 0; y < REGION_SIZE; y++) {
            for (u32 z = 0; z < REGION_SIZE; z++) {
                voxel_type_t type = voxel_types->types[x][y][z];
                voxel_mesh_category_t category = get_voxel_mesh_category(type);

                switch (category) {
                    default: break;
                    case voxel_mesh_category_cube:
                    case voxel_mesh_category_transparent_cube: {
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, right_voxel_types, x, y, z, type, category, voxel_face_right, x, y, z + 1u);
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, left_voxel_types, x, y, z, type, category, voxel_face_left, x, y, z - 1u);
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, top_voxel_types, x, y, z, type, category, voxel_face_top, x, y + 1u, z);
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, bottom_voxel_types, x, y, z, type, category, voxel_face_bottom, x, y - 1u, z);
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, front_voxel_types, x, y, z, type, category, voxel_face_front, x + 1u, y, z);
                        indices.face = add_face_mesh_if_needed(indices.face, voxel_types, back_voxel_types, x, y, z, type, category, voxel_face_back, x - 1u, y, z);
                    } break;
                    case voxel_mesh_category_cross: {
                        building_meshes_arrays.transparent_double_sided[indices.all.transparent_double_sided++] = (voxel_mesh_t){
                            .type = (u8) type,
                            .x = (u8) x,
                            .y = (u8) y,
                            .z = (u8) z
                        };
                    } break;
                }

                if (indices.all.solid >= (NUM_SOLID_BUILDING_MESHES - 6)) {
                    write_meshes_into_display_list(0, indices.all.solid, indices.all.solid * 4, building_meshes_arrays.solid, render_info);
                    indices.all.solid = 0;
                }
                if (indices.all.transparent >= (NUM_TRANSPARENT_BUILDING_MESHES - 6)) {
                    write_meshes_into_display_list(1, indices.all.transparent, indices.all.transparent * 4, building_meshes_arrays.transparent, render_info);
                    indices.all.transparent = 0;
                }
                if (indices.all.transparent_double_sided >= (NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_MESHES - 1)) {
                    write_meshes_into_display_list(2, indices.all.transparent_double_sided, indices.all.transparent_double_sided * 8, building_meshes_arrays.transparent_double_sided, render_info);
                    indices.all.transparent_double_sided = 0;
                }
            }
        }
    }

    if (indices.all.solid > 0) {
        write_meshes_into_display_list(0, indices.all.solid, indices.all.solid * 4, building_meshes_arrays.solid, render_info);
    }
    if (indices.all.transparent > 0) {
        write_meshes_into_display_list(1, indices.all.transparent, indices.all.transparent * 4, building_meshes_arrays.transparent, render_info);
    }
    if (indices.all.transparent_double_sided > 0) {
        write_meshes_into_display_list(2, indices.all.transparent_double_sided, indices.all.transparent_double_sided * 8, building_meshes_arrays.transparent_double_sided, render_info);
    }
}