#pragma once
#include "math.hpp"

namespace math {
    class transform_3d {
        matrix model;
        matrix model_view;

        public:
            transform_3d() = default;
            transform_3d(const transform_3d&) = delete;
            transform_3d& operator=(const transform_3d&) = delete;

            transform_3d(transform_3d&& other);

            void set_position(matrix view, f32 x, f32 y, f32 z);
            inline void update_model_view(matrix view) {
                guMtxConcat(view, model, model_view);
            }

            inline void load(u32 idx) {
                GX_LoadPosMtxImm(model_view, idx);
            }
    };
}