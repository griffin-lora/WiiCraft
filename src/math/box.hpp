#pragma once
#include "math.hpp"

typedef struct {
    glm::vec3 lesser_corner;
    glm::vec3 greater_corner;

} box_t;

bool is_inside_box(box_t box, glm::vec3 pos);
bool do_boxes_collide(box_t box_a, box_t box_b);