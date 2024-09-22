#include "region_management.h"
#include "game/region.h"
#include "game/region_procedural_generation.h"
#include "game/region_visual_generation.h"
#include "game_math.h"
#include <stdlib.h>

alignas(32) size_t num_regions;
alignas(32) region_render_info_t* region_render_infos;
alignas(32) voxel_type_array_t* region_voxel_type_arrays;

void init_region_management(void) {
    num_regions = 1;
    region_render_infos = malloc(num_regions * sizeof(*region_render_infos));
    region_voxel_type_arrays = malloc(num_regions * sizeof(*region_render_infos));
    
    region_render_infos->position = (vec3s) {{ 0.0f, -16.0f, 0.0f }};
    generate_region_voxels((s32vec3s) {{ 0, -1, 0 }}, region_voxel_type_arrays);
    generate_region_visuals(region_voxel_type_arrays, NULL, NULL, NULL, NULL, NULL, NULL, region_render_infos);
}

void manage_regions(s32vec3s, s32vec3s) {
    
}