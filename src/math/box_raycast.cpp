#include "box_raycast.hpp"
#include <algorithm>
#include <functional>

using namespace math;

// Shamelessly stolen from: https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
// I literally have almost no idea what this does.

std::optional<box_raycast> math::get_box_raycast(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& direction_inverse, const box& box) {
    auto t_near = (box.lesser_corner - origin) * direction_inverse;
    auto t_far = (box.greater_corner - origin) * direction_inverse;

    if (std::isnan(t_far.y) || std::isnan(t_far.x)) { return {}; }
    if (std::isnan(t_near.y) || std::isnan(t_near.x)) { return {}; }

    if (t_near.x > t_far.x) { std::swap(t_near.x, t_far.x); }
    if (t_near.y > t_far.y) { std::swap(t_near.y, t_far.y); }
    if (t_near.z > t_far.z) { std::swap(t_near.z, t_far.z); }

    // Maybe remove this
    if (
        t_near.x > t_far.y || t_near.x > t_far.z ||
        t_near.y > t_far.x || t_near.y > t_far.z ||
        t_near.z > t_far.x || t_near.z > t_far.y
    ) { return {}; }

    f32 t_hit_near = std::max({ t_near.x, t_near.y, t_near.z });
    f32 t_hit_far = std::min({ t_far.x, t_far.y, t_far.z });

    if (t_hit_far < 0) { return {}; }

    auto intersection_position = origin + t_hit_near * direction;

    return box_raycast{
        .intersection_position = intersection_position,
        .normal = { 0, 0, 0 },
        .near_hit_time = t_hit_near
    };
}