#pragma once
#include "math.hpp"

namespace gfx {
    struct transform_2d {
        math::matrix model_view;
    };

    void set_position(transform_2d& transform, f32 x, f32 y);

    void load(transform_2d& transform);
}