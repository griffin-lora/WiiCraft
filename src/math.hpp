#pragma once
#include <cstdlib>
#include <array>
#include "math/vector.hpp"
#include "math/matrix.hpp"

namespace math {
    using vector3f = math::vector3<f32>;
    using vector2f = math::vector2<f32>;
    using vector3u8 = math::vector3<u8>;
    using vector3s8 = math::vector3<s8>;
    using vector2u8 = math::vector2<u8>;
    using matrix3f = math::matrix3<f32>;
    using matrix = Mtx;
    using matrix44 = Mtx44;

    template<typename T>
    matrix3<T> from_euler_angles(f32 yaw, f32 pitch, f32 roll) {
        math::matrix3f rotation = {
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 0, 1 }
        };
        f32 cosY = cosf(yaw);
        f32 sinY = sinf(yaw);

        f32 cosP = cosf(pitch);
        f32 sinP = sinf(pitch);

        f32 cosR = cosf(roll);
        f32 sinR = sinf(roll);
        rotation.vec1.x = cosY * cosR + sinY * sinP * sinR;
        rotation.vec2.x = cosR * sinY * sinP - sinR * cosY;
        rotation.vec3.x = cosP * sinY;

        rotation.vec1.y = cosP * sinR;
        rotation.vec2.y = cosR * cosP;
        rotation.vec3.y = -sinP;

        rotation.vec1.z = sinR * cosY * sinP - sinY * cosR;
        rotation.vec2.z = sinY * sinR + cosR * cosY * sinP;
        rotation.vec3.z = cosP * cosY;

        return rotation;
    }
};