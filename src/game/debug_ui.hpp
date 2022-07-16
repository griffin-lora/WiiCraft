#pragma once
#include "gfx/text.hpp"
#include "math/transform_2d.hpp"
#include "chrono.hpp"

namespace game {
    struct debug_ui {
        static constexpr u32 MAT = GX_PNMTX2;

        static constexpr const char* FPS_PREFIX = "FPS: ";
        static constexpr const char* BGT_PREFIX = "BGT: ";
        static constexpr const char* MGT_PREFIX = "MGT: ";
        static constexpr u16 CHAR_SIZE = 16;
        static constexpr u16 PREFIX_WIDTH = gfx::get_text_width(FPS_PREFIX, CHAR_SIZE);

        math::transform_2d tf;
        gfx::display_list fps_prefix_disp_list;
        gfx::display_list bgt_prefix_disp_list;
        gfx::display_list mgt_prefix_disp_list;

        debug_ui();

        void draw(u32 fps, chrono::us block_gen_time, chrono::us mesh_gen_time) const;
    };
}