#pragma once
#include "box.h"

typedef struct {
    bool success;
    vec3s intersection_position;
    vec3s normal;
    f32 near_hit_time;
} box_raycast_t;

typedef struct {
    bool success;
    box_raycast_t val;
} box_raycast_wrap_t;

box_raycast_wrap_t get_box_raycast(vec3s origin, vec3s direction, vec3s direction_inverse, box_t box);