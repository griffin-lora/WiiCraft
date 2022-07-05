#pragma once
#include "math/box_raycast.hpp"
#include "chunk.hpp"
#include "chunk_core.hpp"
#include "block.hpp"

namespace game {
    struct block_raycast {
        math::box_raycast box_raycast;
        world_location location;
        glm::vec3 world_block_position;
    };

    template<typename F1, typename F2>
    std::optional<block_raycast> get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box);
};