#include "box.hpp"

using namespace math;

bool box::is_inside(const glm::vec3& pos) const {
    return pos.x >= lesser_corner.x && pos.y >= lesser_corner.y && pos.z >= lesser_corner.z && pos.x <= greater_corner.x && pos.y <= greater_corner.y && pos.z <= greater_corner.z;
}

bool math::do_boxes_collide(const box& bigger_box, const box& smaller_box) {
    return
        bigger_box.is_inside(smaller_box.lesser_corner) ||
        bigger_box.is_inside(smaller_box.greater_corner);
}