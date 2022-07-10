#pragma once
#include "math/box_raycast.hpp"
#include "chunk.hpp"
#include "chunk_core.hpp"
#include "block.hpp"
#include <vector>

namespace game {
    struct block_raycast {
        world_location location;
        math::box_raycast box_raycast;
        glm::vec3 world_block_position;
    };

    template<typename F1, typename F2, typename F3>
    void handle_block_raycasts_in_region(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box, F3 handle_box_raycast);

    template<typename F1, typename F2>
    std::optional<block_raycast> get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box);

    template<typename F1, typename F2>
    std::vector<block_raycast> get_block_raycasts(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box);
};