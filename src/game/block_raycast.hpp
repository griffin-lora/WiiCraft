#pragma once
#include "math/box_raycast.hpp"
#include "chunk_core.hpp"
#include "block.hpp"
#include "math/vector.h"
#include <vector>
#include <glm/vec3.hpp>

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

block_raycast_wrap_t get_block_raycast(vec3_s32_t corner_pos, glm::vec3 origin, glm::vec3 direction, glm::vec3 begin, glm::vec3 end, glm::vec3 box_transform, block_box_type_t box_type);