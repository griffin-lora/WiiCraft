#include "text.hpp"
#include "text.inl"

using namespace gfx;

void gfx::write_text_into_display_list(gfx::display_list& disp_list, std::string_view str, u16 char_width, u16 char_height) {
    std::size_t vertex_count = str.size() * 4;

    std::size_t disp_list_size = (
        4 + // GX_Begin
        vertex_count * 4 + // GX_Position2u16
        vertex_count * 2 + // GX_TexCoord2u8
        1 // GX_End
    );

    disp_list.resize(disp_list_size);

    disp_list.write_into([&]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        write_text_vertices<u8>([](u16 x, u16 y, u8 u, u8 v) {
            GX_Position2u16(x, y);
            GX_TexCoord2u8(u, v);
        }, str, char_width, char_height);

        GX_End();
    });
}