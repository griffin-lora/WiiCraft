#pragma once
#include "gfx/text.hpp"
#include "math/transform_2d.hpp"
#include "chrono.h"

namespace game {
    struct debug_ui {
        static constexpr u32 mat = GX_PNMTX2;

        static constexpr const char* fps_prefix = "FPS: ";
        static constexpr const char* pos_prefix = "POS: ";
        static constexpr const char* dir_prefix = "DIR: ";
        static constexpr const char* bgt_prefix = "BGT: ";
        static constexpr const char* mgt_prefix = "MGT: ";
        static constexpr const char* mgl_prefix = "MGL: ";
        static constexpr u16 char_size = 16;
        static constexpr u16 prefix_width = gfx::get_text_width(fps_prefix, char_size);

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
        void draw(const glm::vec3& pos, const glm::vec3& dir, us_t total_block_gen_time, us_t total_mesh_gen_time, us_t last_mesh_gen_time, u32 fps) const;
    };
}