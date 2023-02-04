#pragma once
#include "math/box_raycast.hpp"
#include "chunk.hpp"
#include "chunk_core.hpp"
#include "block.hpp"
#include <vector>

typedef struct {
    game::world_location location;
    box_raycast_t box_raycast;
    glm::vec3 world_block_position;
} block_raycast_t;

typedef struct {
    bool success;
    block_raycast_t val;
} block_raycast_wrap_t;

typedef enum {
    block_box_type_collision,
    block_box_type_selection
} block_box_type_t;

block_raycast_wrap_t get_block_raycast(game::chunk::map& chunks, glm::vec3 origin, glm::vec3 direction, glm::vec3 begin, glm::vec3 end, glm::vec3 box_transform, block_box_type_t box_type);