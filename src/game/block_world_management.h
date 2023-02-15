#pragma once
#include "game_math.h"
#include <stddef.h>

void init_block_world(s32vec3s corner_pos);
void manage_block_world(s32vec3s last_corner_pos, s32vec3s corner_pos);
void handle_world_flag_processing(s32vec3s corner_pos);