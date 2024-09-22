#pragma once
#include "game/region.h"
#include <cglm/struct/vec3.h>

void generate_region_visuals(
    const voxel_type_array_t* voxel_types,
    const voxel_type_array_t* front_voxel_types,
    const voxel_type_array_t* back_voxel_types,
    const voxel_type_array_t* top_voxel_types,
    const voxel_type_array_t* bottom_voxel_types,
    const voxel_type_array_t* right_voxel_types,
    const voxel_type_array_t* left_voxel_types,
    region_render_info_t* render_info
);