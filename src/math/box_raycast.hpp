#pragma once
#include "box.hpp"
#include <optional>

typedef struct {
    bool success;
    glm::vec3 intersection_position;
    glm::vec3 normal;
    f32 near_hit_time;
} box_raycast_t;

typedef struct {
    bool success;
    box_raycast_t raycast;
} box_raycast_wrap_t;

box_raycast_wrap_t get_box_raycast(glm::vec3 origin, glm::vec3 direction, glm::vec3 direction_inverse, box_t box);