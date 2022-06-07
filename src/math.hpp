#pragma once
#include <gccore.h>
#include <cstdlib>
#include <array>
#include "math/vector.hpp"

namespace math {
    using vector3f = math::vector3<f32>;
    using vector3u8 = math::vector3<u8>;
    using vector2u8 = math::vector2<u8>;
    using matrix = Mtx;
    using matrix44 = Mtx44;
};