#pragma once
#include "game_math.hpp"
#include <ogc/gu.h>
#include <ogc/gx.h>

namespace math {
    class transform_2d {
        Mtx model_view;

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