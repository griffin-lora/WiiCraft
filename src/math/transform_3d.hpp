#pragma once
#include "game_math.hpp"
#include <ogc/gu.h>
#include <ogc/gx.h>

namespace math {
    class transform_3d {
        Mtx model;
        Mtx model_view;

        public:
            transform_3d() = default;
            transform_3d(const transform_3d&) = delete;
            transform_3d& operator=(const transform_3d&) = delete;

            transform_3d(transform_3d&& other) noexcept;

            void set_position(const Mtx view, f32 x, f32 y, f32 z);
            inline void update_model_view(const Mtx view) {
                guMtxConcat(const_cast<f32(*)[4]>(view), model, model_view);
            }

            inline void load(u32 idx) {
                GX_LoadPosMtxImm(model_view, idx);
            }
    };
}