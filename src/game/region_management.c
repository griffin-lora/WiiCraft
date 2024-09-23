#include "region_management.h"
#include "game/region.h"
#include "game/region_procedural_generation.h"
#include "game/region_visual_generation.h"
#include "game_math.h"
#include "log.h"
#include <cglm/ivec3.h>
#include <string.h>

alignas(32) u32 world_size;
s32vec3s corner_region_pos;
alignas(32) voxel_type_array_t** region_voxel_type_arrays;
alignas(32) region_render_info_t* region_render_infos;

u32vec3s get_region_relative_position(s32vec3s region_pos) {
    return (u32vec3s) {{
        (u32) (region_pos.x - corner_region_pos.x),
        (u32) (region_pos.y - corner_region_pos.y),
        (u32) (region_pos.z - corner_region_pos.z)
    }};
}

static s32vec3s get_region_position(u32vec3s region_rel_pos) {
    return (s32vec3s) {{
        (s32) region_rel_pos.x + corner_region_pos.x,
        (s32) region_rel_pos.y + corner_region_pos.y,
        (s32) region_rel_pos.z + corner_region_pos.z
    }};
}

bool is_region_relative_position_out_of_bounds(u32vec3s region_rel_pos) {
    return region_rel_pos.x >= world_size || region_rel_pos.y >= world_size || region_rel_pos.z >= world_size;
}

static const voxel_type_array_t* get_neighbor_voxel_type_array(u32vec3s region_rel_pos) {
	REGION_TYPE_3D(const voxel_type_array_t*) voxel_type_arrays = REGION_CAST_3D(const voxel_type_array_t*, region_voxel_type_arrays);
    if (is_region_relative_position_out_of_bounds(region_rel_pos)) {
        return NULL;
    }
    return (*voxel_type_arrays)[region_rel_pos.x][region_rel_pos.y][region_rel_pos.z];
}

void init_region_management(void) {
    world_size = 6;
    region_voxel_type_arrays = malloc(get_num_regions() * sizeof(voxel_type_array_t*));
    region_render_infos = malloc(get_num_regions() * sizeof(*region_render_infos));

    memset(region_voxel_type_arrays, 0, get_num_regions() * sizeof(voxel_type_array_t*));
    memset(region_render_infos, 0, get_num_regions() * sizeof(*region_render_infos));

    for (size_t i = 0; i < get_num_regions(); i++) {
        region_voxel_type_arrays[i] = malloc(sizeof(*region_voxel_type_arrays[i]));
    }
    
	REGION_TYPE_3D(voxel_type_array_t*) voxel_type_arrays = REGION_CAST_3D(voxel_type_array_t*, region_voxel_type_arrays);
	REGION_TYPE_3D(region_render_info_t) render_infos = REGION_CAST_3D(region_render_info_t, region_render_infos);

	for (u32 x = 0; x < world_size; x++) {
		for (u32 y = 0; y < world_size; y++) {
			for (u32 z = 0; z < world_size; z++) {
                s32vec3s region_pos = get_region_position((u32vec3s) {{ x, y, z }});

                voxel_type_array_t* voxel_types = (*voxel_type_arrays)[x][y][z];
                generate_region_voxels(region_pos, voxel_types);
            }
        }
    }

	for (u32 x = 0; x < world_size; x++) {
		for (u32 y = 0; y < world_size; y++) {
			for (u32 z = 0; z < world_size; z++) {
                const voxel_type_array_t* voxel_types = (*voxel_type_arrays)[x][y][z];
                region_render_info_t* render_info = &(*render_infos)[x][y][z];

                generate_region_visuals(
                    voxel_types,
                    get_neighbor_voxel_type_array((u32vec3s) {{ x + 1u, y, z }}), 
                    get_neighbor_voxel_type_array((u32vec3s) {{ x - 1u, y, z }}), 
                    get_neighbor_voxel_type_array((u32vec3s) {{ x, y + 1u, z }}), 
                    get_neighbor_voxel_type_array((u32vec3s) {{ x, y - 1u, z }}), 
                    get_neighbor_voxel_type_array((u32vec3s) {{ x, y, z + 1u }}), 
                    get_neighbor_voxel_type_array((u32vec3s) {{ x, y, z - 1u }}), 
                    render_info
                );
            }
        }
    }
}

void manage_regions(s32vec3s, s32vec3s) {
    
}