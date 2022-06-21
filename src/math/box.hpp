#pragma once
#include "math.hpp"

namespace math {
    struct box {
        glm::vec3 lesser_corner;
        glm::vec3 greater_corner;

        bool is_inside(const glm::vec3& pos) const;
    };
}