#pragma once
#include "math.hpp"

namespace gfx {
    struct transform_3d {
        math::matrix model;
        math::matrix model_view;
    };

    void set_position(transform_3d& transform, math::matrix view, f32 x, f32 y, f32 z);

    void load(transform_3d& transform);

    inline void update_model_view(transform_3d& transform, math::matrix view) {
        guMtxConcat(view, transform.model, transform.model_view);
    }
}