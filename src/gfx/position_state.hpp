#pragma once
#include "math.hpp"

namespace gfx {
    struct position_state {
        math::matrix model;
        math::matrix model_view;
    };

    void init(position_state& pos_state, math::matrix view, f32 x, f32 y, f32 z);

    void load(position_state& pos_state);

    inline void update_model_view(position_state& pos_state, math::matrix view) {
        guMtxConcat(view, pos_state.model, pos_state.model_view);
    }
}