#pragma once
#include "game_math.h"
#include "math/box_raycast.h"
#include "block_world_core.h"
#include "block.h"

typedef struct {
    world_location_t location;
    box_raycast_t box_raycast;
    vec3s world_block_position;
} block_raycast_t;

typedef struct {
    bool success;
    block_raycast_t val;
} block_raycast_wrap_t;

typedef enum __attribute__((__packed__)) {
    block_box_type_collision,
    block_box_type_selection
} block_box_type_t;

block_raycast_wrap_t get_block_raycast(s32vec3s corner_pos, vec3s origin, vec3s direction, vec3s begin, vec3s end, vec3s box_transform, block_box_type_t box_type);