#pragma once
#include "game_math.h"

void init_region_management(void);
void manage_regions(s32vec3s last_region_pos, s32vec3s region_pos);

u32vec3s get_region_relative_position(s32vec3s region_pos);
bool is_region_relative_position_out_of_bounds(u32vec3s region_rel_pos);