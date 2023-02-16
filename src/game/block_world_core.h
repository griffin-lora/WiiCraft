#pragma once
#include "pool.h"
#include <stdbool.h>

typedef struct {
    s32vec3s ch_pos;
    u8vec3s bl_pos;
    u8 chunk_index;
    block_type_t* bl_tp;
} world_location_t;

typedef struct {
    bool success;
    world_location_t val;
} world_location_wrap_t;

world_location_wrap_t get_world_location_at_world_position(s32vec3s corner_pos, vec3s pos);
bool update_block_chunk_and_neighbors(s32vec3s corner_pos, s32vec3s chunk_pos, u8vec3s modification_pos);