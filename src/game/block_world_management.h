#pragma once
#include "math/vector.h"
#include <stddef.h>

void init_block_world(vec3_s32_t corner_pos);
void manage_block_world(vec3_s32_t last_corner_pos, vec3_s32_t corner_pos);
void handle_world_flag_processing(vec3_s32_t corner_pos);