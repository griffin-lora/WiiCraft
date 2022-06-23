#pragma once
#include "gfx.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_2d.hpp"
#include "math.hpp"

namespace game {
    struct cursor {
        math::transform_2d tf;
        gfx::display_list disp_list;

        cursor();

        void draw() const;
    };
};