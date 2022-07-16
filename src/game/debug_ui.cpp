#include "debug_ui.hpp"
#include <string>

#include "gfx/text.inl"

using namespace game;

debug_ui::debug_ui() {
    tf.set_position(10.0f, 30.0f);
    tf.load(MAT);
    gfx::write_text_into_standard_display_list(fps_prefix_disp_list, FPS_PREFIX, CHAR_SIZE, CHAR_SIZE);
    gfx::write_text_into_display_list<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x, y + CHAR_SIZE);
        GX_TexCoord2u8(u, v);
    }, bgt_prefix_disp_list, BGT_PREFIX, CHAR_SIZE, CHAR_SIZE);
    gfx::write_text_into_display_list<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x, y + (CHAR_SIZE * 2));
        GX_TexCoord2u8(u, v);
    }, mgt_prefix_disp_list, MGT_PREFIX, CHAR_SIZE, CHAR_SIZE);
}

void debug_ui::draw(u32 fps, chrono::us block_gen_time, chrono::us mesh_gen_time) const {
    GX_SetCurrentMtx(MAT);

    fps_prefix_disp_list.call();
    bgt_prefix_disp_list.call();
    mgt_prefix_disp_list.call();

    auto fps_str = std::to_string(fps);
    auto block_gen_time_str = std::to_string(block_gen_time);
    auto mesh_gen_time_str = std::to_string(mesh_gen_time);
    std::size_t vertex_count = 4 * (fps_str.size() + block_gen_time_str.size() + mesh_gen_time_str.size());
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

    gfx::write_text_vertices<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x + PREFIX_WIDTH, y);
        GX_TexCoord2u8(u, v);
    }, fps_str, CHAR_SIZE, CHAR_SIZE);

    gfx::write_text_vertices<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x + PREFIX_WIDTH, y + CHAR_SIZE);
        GX_TexCoord2u8(u, v);
    }, block_gen_time_str, CHAR_SIZE, CHAR_SIZE);

    gfx::write_text_vertices<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x + PREFIX_WIDTH, y + (CHAR_SIZE * 2));
        GX_TexCoord2u8(u, v);
    }, mesh_gen_time_str, CHAR_SIZE, CHAR_SIZE);

    GX_End();
}