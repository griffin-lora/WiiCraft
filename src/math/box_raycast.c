#include "box_raycast.h"
#include <math.h>

// Shamelessly stolen from: https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
// I literally have almost no idea what this does.

box_raycast_wrap_t get_box_raycast(vec3s origin, vec3s direction, vec3s direction_inverse, box_t box) {
    vec3s t_near = glms_vec3_mul(glms_vec3_sub(box.lesser_corner, origin), direction_inverse);
    vec3s t_far = glms_vec3_mul(glms_vec3_sub(box.greater_corner, origin), direction_inverse);

    if (isnan(t_far.y) || isnan(t_far.x)) { return (box_raycast_wrap_t){ false }; }
    if (isnan(t_near.y) || isnan(t_near.x)) { return (box_raycast_wrap_t){ false }; }

    f32 temp;
    if (t_near.x > t_far.x) {
        temp = t_near.x;
        t_near.x = t_far.x;
        t_far.x = temp;
    }
    if (t_near.y > t_far.y) {
        temp = t_near.y;
        t_near.y = t_far.y;
        t_far.y = temp;
    }
    if (t_near.z > t_far.z) {
        temp = t_near.z;
        t_near.z = t_far.z;
        t_far.z = temp;
    }

    if (
        t_near.x > t_far.y || t_near.x > t_far.z ||
        t_near.y > t_far.x || t_near.y > t_far.z ||
        t_near.z > t_far.x || t_near.z > t_far.y
    ) { return (box_raycast_wrap_t){ false }; }

    f32 t_hit_near = fmaxf(fmaxf(t_near.x, t_near.y), t_near.z);
    f32 t_hit_far = fminf(fminf(t_far.x, t_far.y), t_far.z);

    if (t_hit_far < 0) { return (box_raycast_wrap_t){ false }; }

    vec3s intersection_position = glms_vec3_add(origin, glms_vec3_scale(direction, t_hit_near));

    vec3s normal = { .x = 0, .y = 0, .z = 0 };
    if (t_near.x > t_near.y && t_near.x > t_near.z) { normal = (vec3s){ .x = direction_inverse.x < 0 ? 1 : -1, .y = 0, .z = 0 }; }
    else if (t_near.y > t_near.x && t_near.y > t_near.z) { normal = (vec3s){ .x = 0, .y = direction_inverse.y < 0 ? 1 : -1, .z = 0 }; }
    else if (t_near.z > t_near.x && t_near.z > t_near.y) { normal = (vec3s){ .x = 0, .y = 0, .z = direction_inverse.z < 0 ? 1 : -1 }; }

    return (box_raycast_wrap_t){
        .success = true,
        .val = {
            .intersection_position = intersection_position,
            .normal = normal,
            .near_hit_time = t_hit_near
        }
    };
}