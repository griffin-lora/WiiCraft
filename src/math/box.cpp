#include "box.hpp"

using namespace math;

bool box::is_inside(const glm::vec3& pos) const {
    return pos.x >= lesser_corner.x && pos.y >= lesser_corner.y && pos.z >= lesser_corner.z && pos.x <= greater_corner.x && pos.y <= greater_corner.y && pos.z <= greater_corner.z;
}

bool math::do_boxes_collide(const box& box_a, const box& box_b) {
    return
        (box_a.lesser_corner.x <= box_b.greater_corner.x && box_a.greater_corner.x >= box_b.lesser_corner.x) &&
        (box_a.lesser_corner.y <= box_b.greater_corner.y && box_a.greater_corner.y >= box_b.lesser_corner.y) &&
        (box_a.lesser_corner.z <= box_b.greater_corner.z && box_a.greater_corner.z >= box_b.lesser_corner.z);
}