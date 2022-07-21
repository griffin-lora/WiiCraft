#pragma once
#include "gfx/text.hpp"
#include "math/transform_2d.hpp"
#include "chrono.hpp"

namespace game {
    struct debug_ui {
        static constexpr u32 MAT = GX_PNMTX2;

        static constexpr const char* FPS_PREFIX = "FPS: ";
        static constexpr const char* POS_PREFIX = "POS: ";
        static constexpr const char* DIR_PREFIX = "DIR: ";
        static constexpr const char* BGT_PREFIX = "BGT: ";
        static constexpr const char* MGT_PREFIX = "MGT: ";
        static constexpr const char* MGL_PREFIX = "MGL: ";
        static constexpr u16 CHAR_SIZE = 16;
        static constexpr u16 PREFIX_WIDTH = gfx::get_text_width(FPS_PREFIX, CHAR_SIZE);

        math::transform_2d tf;
        gfx::display_list fps_prefix_disp_list;
        gfx::display_list pos_prefix_disp_list;
        gfx::display_list dir_prefix_disp_list;
        gfx::display_list bgt_prefix_disp_list;
        gfx::display_list mgt_prefix_disp_list;
        gfx::display_list mgl_prefix_disp_list;

        bool draw_extra_info = false;

        debug_ui();

        void update(u32 buttons_down);
        void draw(const glm::vec3& pos, const glm::vec3& dir, chrono::us total_block_gen_time, chrono::us total_mesh_gen_time, chrono::us last_mesh_gen_time, u32 fps) const;
    };
}