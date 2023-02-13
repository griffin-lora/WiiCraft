#pragma once
#include "pool.h"
#include <stdbool.h>

typedef struct {
    vec3_s32_t ch_pos;
    vec3_u8_t bl_pos;
    u8 chunk_index;
    block_type_t* bl_tp;
} world_location_t;

typedef struct {
    bool success;
    world_location_t val;
} world_location_wrap_t;

world_location_wrap_t get_world_location_at_world_position(vec3_s32_t corner_pos, vec3s pos);
bool update_block_chunk_and_neighbors(vec3_s32_t corner_pos, vec3_s32_t chunk_pos);