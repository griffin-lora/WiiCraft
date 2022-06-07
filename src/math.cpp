#include "math.hpp"

using namespace math;

matrix3f math::from_euler_angles(f32 yaw, f32 pitch, f32 roll) {
    math::matrix3f rotation = {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }
    };
    f32 cos_y = cosf(yaw);
    f32 sin_y = sinf(yaw);

    f32 cos_p = cosf(pitch);
    f32 sin_p = sinf(pitch);

    f32 cos_r = cosf(roll);
    f32 sin_r = sinf(roll);
    rotation.vec1.x = cos_y * cos_r + sin_y * sin_p * sin_r;
    rotation.vec2.x = cos_r * sin_y * sin_p - sin_r * cos_y;
    rotation.vec3.x = cos_p * sin_y;

    rotation.vec1.y = cos_p * sin_r;
    rotation.vec2.y = cos_r * cos_p;
    rotation.vec3.y = -sin_p;

    rotation.vec1.z = sin_r * cos_y * sin_p - sin_y * cos_r;
    rotation.vec2.z = sin_y * sin_r + cos_r * cos_y * sin_p;
    rotation.vec3.z = cos_p * cos_y;

    return rotation;
}