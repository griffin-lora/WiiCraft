#include "box.hpp"

using namespace math;

bool box::is_inside(const glm::vec3& pos) {
    return pos.x >= lesser_corner.x && pos.y >= lesser_corner.y && pos.z >= lesser_corner.z && pos.x <= greater_corner.x && pos.y <= greater_corner.y && pos.z <= greater_corner.z;
}