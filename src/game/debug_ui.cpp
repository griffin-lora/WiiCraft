#include "debug_ui.hpp"
#include <string>

#include "gfx/text.inl"

using namespace game;

debug_ui::debug_ui() {
    fps_tf.set_position(10.0f, 30.0f);
    gfx::write_text_into_display_list(fps_prefix_disp_list, FPS_PREFIX, CHAR_SIZE, CHAR_SIZE);
}

void debug_ui::draw(u32 fps) const {
    fps_tf.load(GX_PNMTX3);

    fps_prefix_disp_list.call();

    auto fps_str = std::to_string(fps);
    std::size_t vertex_count = fps_str.size() * 4;
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

    gfx::write_text_vertices<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x + FPS_PREFIX_WIDTH, y);
        GX_TexCoord2u8(u, v);
    }, fps_str, CHAR_SIZE, CHAR_SIZE);

    GX_End();
}