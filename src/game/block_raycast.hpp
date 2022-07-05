#pragma once
#include "math/box_raycast.hpp"
#include "chunk.hpp"
#include "chunk_core.hpp"
#include "block.hpp"

namespace game {
    struct block_raycast {
        math::box_raycast box_raycast;
        world_location location;
    };

    template<typename F>
    std::optional<block_raycast> get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& end, F get_boxes);
};