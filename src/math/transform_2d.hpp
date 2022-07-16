#pragma once
#include "math.hpp"

namespace math {
    class transform_2d {
        matrix model_view;

        public:
            transform_2d() = default;
            transform_2d(const transform_2d&) = delete;
            transform_2d& operator=(const transform_2d&) = delete;

            transform_2d(transform_2d&& other) noexcept;

            void set_position(f32 x, f32 y);

            inline void load(u32 idx) {
                GX_LoadPosMtxImm(model_view, idx);
            }
    };
}