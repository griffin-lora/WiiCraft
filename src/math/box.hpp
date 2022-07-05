#pragma once
#include "math.hpp"

namespace math {
    struct box {
        const glm::vec3 lesser_corner;
        const glm::vec3 greater_corner;
        const glm::vec3 size;
        const glm::vec3 center;

        constexpr box(const glm::vec3& lesser_corner, const glm::vec3& greater_corner) :
            lesser_corner(lesser_corner),
            greater_corner(greater_corner),
            size(greater_corner - lesser_corner),
            center(lesser_corner + (size * 0.5f))
        {}

        bool is_inside(const glm::vec3& pos) const;
    };

    bool do_boxes_collide(const box& box_a, const box& box_b);
}