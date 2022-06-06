#pragma once
#include <gccore.h>
#include <cstdlib>
#include <array>

namespace math {
    using vector3 = guVector;
    struct vector3u8 {
        u8 x;
        u8 y;
        u8 z;
    };
    struct vector2u8 {
        u8 x;
        u8 y;
    };
    using matrix = Mtx;
    using matrix44 = Mtx44;
};