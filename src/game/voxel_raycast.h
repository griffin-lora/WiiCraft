#pragma once
#include "game_math.h"
#include "math/box_raycast.h"
#include "world_location.h"
#include "voxel.h"

typedef struct {
    world_location_t location;
    box_raycast_t box_raycast;
} voxel_raycast_t;

typedef struct {
    bool success;
    voxel_raycast_t val;
} voxel_raycast_wrap_t;

typedef enum {
    voxel_box_type_collision,
    voxel_box_type_selection
} voxel_box_type_t;

voxel_raycast_wrap_t get_voxel_raycast(s32vec3s region_pos, vec3s origin, vec3s direction, vec3s begin, vec3s end, vec3s box_transform, voxel_box_type_t box_type);