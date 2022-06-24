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

            transform_3d(transform_3d&& other) noexcept;

            void set_position(const matrix view, f32 x, f32 y, f32 z);
            inline void update_model_view(const matrix view) {
                guMtxConcat(const_cast<f32(*)[4]>(view), model, model_view);
            }

            inline void load(u32 idx) const {
                GX_LoadPosMtxImm(const_cast<f32(*)[4]>(model_view), idx);
            }
    };
}