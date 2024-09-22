#pragma once
#include "game/voxel.h"
#include "game_math.h"
#include <stdbool.h>

typedef struct {
    s32vec3s region_pos;
    u8vec3s voxel_local_pos;
    voxel_type_t* voxel_type;
} world_location_t;

typedef struct {
    bool success;
    world_location_t val;
} world_location_wrap_t;

world_location_wrap_t get_world_location_at_voxel_world_position(s32vec3s region_pos, vec3s voxel_world_pos);
vec3s get_world_position_from_world_location(const world_location_t* world_loc);