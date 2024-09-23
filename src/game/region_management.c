#include "region_management.h"
#include "game/region.h"
#include "game/region_procedural_generation.h"
#include "game/region_visual_generation.h"
#include "game_math.h"
#include "log.h"
#include <string.h>

alignas(32) u32 world_size;
alignas(32) region_render_info_t* region_render_infos;
alignas(32) voxel_type_array_t* region_voxel_type_arrays;

static const voxel_type_array_t* get_neighbor_voxel_type_array(u32 nx, u32 ny, u32 nz) {
	REGION_TYPE_3D(const voxel_type_array_t) voxel_type_arrays = REGION_CAST_3D(const voxel_type_array_t, region_voxel_type_arrays);
    if (nx >= world_size || ny >= world_size || nz >= world_size) {
        return NULL;
    }
    return &(*voxel_type_arrays)[nx][ny][nz];
}

void init_region_management(void) {
    world_size = 6;
    region_render_infos = malloc(get_num_regions() * sizeof(*region_render_infos));
    region_voxel_type_arrays = malloc(get_num_regions() * sizeof(*region_voxel_type_arrays));

    memset(region_voxel_type_arrays, 0, get_num_regions() * sizeof(*region_voxel_type_arrays));
    memset(region_render_infos, 0, get_num_regions() * sizeof(*region_render_infos));
    
	REGION_TYPE_3D(voxel_type_array_t) voxel_type_arrays = REGION_CAST_3D(voxel_type_array_t, region_voxel_type_arrays);
	REGION_TYPE_3D(region_render_info_t) render_infos = REGION_CAST_3D(region_render_info_t, region_render_infos);

	for (u32 x = 0; x < world_size; x++) {
		for (u32 y = 0; y < world_size; y++) {
			for (u32 z = 0; z < world_size; z++) {
                voxel_type_array_t* voxel_types = &(*voxel_type_arrays)[x][y][z];
                region_render_info_t* render_info = &(*render_infos)[x][y][z];

                generate_region_voxels((s32vec3s) {{ (s32) x, (s32) y, (s32) z }}, voxel_types);
                generate_region_visuals(
                    voxel_types,
                    get_neighbor_voxel_type_array(x + 1u, y, z), 
                    get_neighbor_voxel_type_array(x - 1u, y, z), 
                    get_neighbor_voxel_type_array(x, y + 1u, z), 
                    get_neighbor_voxel_type_array(x, y - 1u, z), 
                    get_neighbor_voxel_type_array(x, y, z + 1u), 
                    get_neighbor_voxel_type_array(x, y, z - 1u), 
                    render_info
                );
            }
        }
    }
}

void manage_regions(s32vec3s, s32vec3s) {
    
}