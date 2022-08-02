#pragma once
#include "gfx.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_2d.hpp"
#include "math.hpp"
#include "camera.hpp"
#include "chunk.hpp"

namespace game {
    struct water_overlay {
        static constexpr u32 mat = GX_PNMTX1;

        math::transform_2d tf;
        gfx::display_list disp_list;

        water_overlay();

        void draw(const camera& cam, const chunk::map& chunks) const;
    };
}