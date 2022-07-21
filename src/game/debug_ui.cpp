#include "debug_ui.hpp"
#include <string>
#include <sstream>

#include "gfx/text.inl"
#include "input.hpp"

using namespace game;

debug_ui::debug_ui() {
    tf.set_position(10.0f, 20.0f);
    tf.load(MAT);

    constexpr auto write_text = [](gfx::display_list& disp_list, std::string_view str, u16 y_offset) {
        gfx::write_text_into_display_list<u8>([y_offset](u16 x, u16 y, u8 u, u8 v) {
            GX_Position2u16(x, y + (CHAR_SIZE * y_offset));
            GX_TexCoord2u8(u, v);
        }, disp_list, str, CHAR_SIZE, CHAR_SIZE);
    };

    write_text(fps_prefix_disp_list, FPS_PREFIX, 0);
    write_text(pos_prefix_disp_list, POS_PREFIX, 1);
    write_text(dir_prefix_disp_list, DIR_PREFIX, 2);
    write_text(bgt_prefix_disp_list, BGT_PREFIX, 3);
    write_text(mgt_prefix_disp_list, MGT_PREFIX, 4);
    write_text(mgl_prefix_disp_list, MGL_PREFIX, 5);
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

void debug_ui::draw(const glm::vec3& pos, const glm::vec3& dir, chrono::us total_block_gen_time, chrono::us total_mesh_gen_time, chrono::us last_mesh_gen_time, u32 fps) const {
    constexpr auto write_text = [](std::string_view str, u16 y_offset) {
        gfx::write_text_vertices<u8>([y_offset](u16 x, u16 y, u8 u, u8 v) {
            GX_Position2u16(x + PREFIX_WIDTH, y + (CHAR_SIZE * y_offset));
            GX_TexCoord2u8(u, v);
        }, str, CHAR_SIZE, CHAR_SIZE);
    };

    GX_SetCurrentMtx(MAT);

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
        std::size_t vertex_count = 4 * (fps_str.size() + pos_str.size() + dir_str.size() + total_block_gen_time_str.size() + total_mesh_gen_time_str.size() + last_mesh_gen_time_str.size());

        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        write_text(fps_str, 0);
        write_text(pos_str, 1);
        write_text(dir_str, 2);
        write_text(total_block_gen_time_str, 3);
        write_text(total_mesh_gen_time_str, 4);
        write_text(last_mesh_gen_time_str, 5);

        GX_End();
    } else {
        std::size_t vertex_count = 4 * fps_str.size();

        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
        write_text(fps_str, 0);
        GX_End();
    }
}