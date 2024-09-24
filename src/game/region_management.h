#pragma once
#include "game/voxel.h"
#include "game_math.h"

void init_region_management(void);
void manage_regions(s32vec3s last_region_pos, s32vec3s region_pos);

u32vec3s get_region_relative_position(s32vec3s region_pos);
bool is_region_relative_position_out_of_bounds(u32vec3s region_rel_pos);

// Returns NULL if there is no valid voxel at the given voxel world position
voxel_type_t* get_voxel_type_from_voxel_world_position(s32vec3s voxel_world_pos);