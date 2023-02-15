#pragma once
#include "game_math.h"

typedef struct {
    vec3s lesser_corner;
    vec3s greater_corner;
} box_t;

bool is_inside_box(box_t box, vec3s pos);
bool do_boxes_collide(box_t box_a, box_t box_b);