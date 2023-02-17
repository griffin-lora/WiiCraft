#include "debug_ui.hpp"
extern "C" {
    #include "debug_ui.h"
}
#include <string>
#include <sstream>
#include <wiiuse/wpad.h>

#include "gfx/text.inl"
#include "game_math.h"

extern vec3s character_position;
extern vec3s cam_look;

using namespace game;

debug_ui::debug_ui() {
    tf.set_position(10.0f, 20.0f);
    tf.load(mat);

    constexpr auto write_text = [](gfx::display_list& disp_list, std::string_view str, u16 y_offset) {
        gfx::write_text_into_display_list<u8>([y_offset](u16 x, u16 y, u8 u, u8 v) {
            GX_Position2u16(x, y + (char_size * y_offset));
            GX_TexCoord2u8(u, v);
        }, disp_list, str, char_size, char_size);
    };

    write_text(fps_prefix_disp_list, fps_prefix, 0);
    write_text(pos_prefix_disp_list, pos_prefix, 1);
    write_text(dir_prefix_disp_list, dir_prefix, 2);
    write_text(bgt_prefix_disp_list, bgt_prefix, 3);
    write_text(mgt_prefix_disp_list, mgt_prefix, 4);
    write_text(mgl_prefix_disp_list, mgl_prefix, 5);
}

static inline std::string to_string(const glm::vec3& v) {
    std::stringstream ss;
    ss << v.x << ' ' << v.y << ' ' << v.z;
    return ss.str();
}

void debug_ui::update(u32 buttons_down) {
    if (buttons_down & WPAD_BUTTON_2) {
        draw_extra_info = !draw_extra_info;
    }
}

void debug_ui::draw(const glm::vec3& pos, const glm::vec3& dir, us_t total_block_gen_time, us_t total_mesh_gen_time, us_t last_mesh_gen_time, u32 fps) const {
    constexpr auto write_text = [](std::string_view str, u16 y_offset) {
        gfx::write_text_vertices<u8>([y_offset](u16 x, u16 y, u8 u, u8 v) {
            GX_Position2u16(x + prefix_width, y + (char_size * y_offset));
            GX_TexCoord2u8(u, v);
        }, str, char_size, char_size);
    };

    GX_SetCurrentMtx(mat);

    fps_prefix_disp_list.call();
    auto fps_str = std::to_string(fps);

    if (draw_extra_info) {
        pos_prefix_disp_list.call();
        dir_prefix_disp_list.call();
        bgt_prefix_disp_list.call();
        mgt_prefix_disp_list.call();
        mgl_prefix_disp_list.call();

        auto pos_str = to_string(pos);
        auto dir_str = to_string(dir);
        auto total_block_gen_time_str = std::to_string(total_block_gen_time);
        auto total_mesh_gen_time_str = std::to_string(total_mesh_gen_time);
        auto last_mesh_gen_time_str = std::to_string(last_mesh_gen_time);
        std::size_t num_vertices = 4 * (fps_str.size() + pos_str.size() + dir_str.size() + total_block_gen_time_str.size() + total_mesh_gen_time_str.size() + last_mesh_gen_time_str.size());

        GX_Begin(GX_QUADS, GX_VTXFMT2, num_vertices);

        write_text(fps_str, 0);
        write_text(pos_str, 1);
        write_text(dir_str, 2);
        write_text(total_block_gen_time_str, 3);
        write_text(total_mesh_gen_time_str, 4);
        write_text(last_mesh_gen_time_str, 5);

        GX_End();
    } else {
        std::size_t num_vertices = 4 * fps_str.size();

        GX_Begin(GX_QUADS, GX_VTXFMT2, num_vertices);
        write_text(fps_str, 0);
        GX_End();
    }
}

static u8 ui_buf[sizeof(debug_ui)];
static debug_ui* ui;

void debug_ui_init(void) {
    ui = new (ui_buf) debug_ui();
}

void debug_ui_update(u32 buttons_down) {
    ui->update(buttons_down);
}

void debug_ui_draw(us_t total_block_gen_time, us_t total_mesh_gen_time, us_t last_mesh_gen_time, u32 fps) {
    ui->draw({ character_position.raw[0], character_position.raw[1], character_position.raw[2] }, { cam_look.raw[0], cam_look.raw[1], cam_look.raw[2] }, total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time, fps);
}