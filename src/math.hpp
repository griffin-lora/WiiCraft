#pragma once
#include <cstdlib>
#include <array>
#include "math/vector.hpp"
#include "math/matrix.hpp"

namespace math {
    using vector3f = math::vector3<f32>;
    using vector3s = math::vector3<s32>;
    using vector2f = math::vector2<f32>;
    using vector3u8 = math::vector3<u8>;
    using vector3s8 = math::vector3<s8>;
    using vector2u8 = math::vector2<u8>;
    using matrix3f = math::matrix3<f32>;
    using matrix = Mtx;
    using matrix44 = Mtx44;

    matrix3f from_euler_angles(f32 yaw, f32 pitch, f32 roll);
};