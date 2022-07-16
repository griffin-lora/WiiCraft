#pragma once
#include "gfx/text.hpp"
#include "math/transform_2d.hpp"

namespace game {
    struct debug_ui {
        static constexpr u32 FPS_MAT = GX_PNMTX2;

        static constexpr const char* FPS_PREFIX = "FPS: ";
        static constexpr u16 CHAR_SIZE = 16;
        static constexpr u16 FPS_PREFIX_WIDTH = gfx::get_text_width(FPS_PREFIX, CHAR_SIZE);

        math::transform_2d fps_tf;
        gfx::display_list fps_prefix_disp_list;

        debug_ui();

        void draw(u32 fps) const;
    };
};