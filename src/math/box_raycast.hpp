#pragma once
#include "box.hpp"
#include <optional>

namespace math {
    struct box_raycast {
        glm::vec3 intersection_position;
        glm::vec3 normal;
        f32 near_hit_time;
    };

    std::optional<box_raycast> get_box_raycast(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& direction_inverse, const box& box);
};