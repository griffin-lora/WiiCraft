#include "box_raycast.hpp"
#include <algorithm>
#include <functional>

// Shamelessly stolen from: https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
// I literally have almost no idea what this does.

box_raycast_wrap_t get_box_raycast(glm::vec3 origin, glm::vec3 direction, glm::vec3 direction_inverse, box_t box) {
    auto t_near = (box.lesser_corner - origin) * direction_inverse;
    auto t_far = (box.greater_corner - origin) * direction_inverse;

    if (std::isnan(t_far.y) || std::isnan(t_far.x)) { return box_raycast_wrap_t{ false }; }
    if (std::isnan(t_near.y) || std::isnan(t_near.x)) { return box_raycast_wrap_t{ false }; }

    if (t_near.x > t_far.x) { std::swap(t_near.x, t_far.x); }
    if (t_near.y > t_far.y) { std::swap(t_near.y, t_far.y); }
    if (t_near.z > t_far.z) { std::swap(t_near.z, t_far.z); }

    if (
        t_near.x > t_far.y || t_near.x > t_far.z ||
        t_near.y > t_far.x || t_near.y > t_far.z ||
        t_near.z > t_far.x || t_near.z > t_far.y
    ) { return box_raycast_wrap_t{ false }; }

    f32 t_hit_near = std::max({ t_near.x, t_near.y, t_near.z });
    f32 t_hit_far = std::min({ t_far.x, t_far.y, t_far.z });

    if (t_hit_far < 0) { return box_raycast_wrap_t{ false }; }

    auto intersection_position = origin + t_hit_near * direction;

    return box_raycast_wrap_t{
        .success = true,
        .val = {
            .intersection_position = intersection_position,
            .normal = [&direction_inverse, &t_near]() -> glm::vec3 {
                if (t_near.x > t_near.y && t_near.x > t_near.z) { return { direction_inverse.x < 0 ? 1 : -1, 0, 0 }; }
                else if (t_near.y > t_near.x && t_near.y > t_near.z) { return { 0, direction_inverse.y < 0 ? 1 : -1, 0 }; }
                else if (t_near.z > t_near.x && t_near.z > t_near.y) { return { 0, 0, direction_inverse.z < 0 ? 1 : -1 }; }
                return { 0, 0, 0 };
            }(),
            .near_hit_time = t_hit_near
        }
    };
}