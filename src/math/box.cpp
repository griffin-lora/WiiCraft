#include "box.hpp"

using namespace math;

bool box::is_inside(const glm::vec3& pos) const {
    return pos.x >= lesser_corner.x && pos.y >= lesser_corner.y && pos.z >= lesser_corner.z && pos.x <= greater_corner.x && pos.y <= greater_corner.y && pos.z <= greater_corner.z;
}

bool math::do_boxes_collide(const box& box_a, const box& box_b) {
    return
        box_a.is_inside(box_b.lesser_corner) ||
        box_a.is_inside(box_b.greater_corner) ||
        box_b.is_inside(box_a.lesser_corner) ||
        box_b.is_inside(box_a.greater_corner);
}