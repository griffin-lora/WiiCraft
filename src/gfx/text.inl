#pragma once
#include "text.hpp"

template<typename U, typename F, typename P>
void gfx::write_text_into_display_list(F write_vertex, gfx::display_list& disp_list, std::string_view str, P char_width, P char_height) {
    std::size_t vertex_count = str.size() * 4;

    std::size_t disp_list_size = (
        (vertex_count > 0xff ? 4 : 3) + // GX_Begin
        vertex_count * 4 + // GX_Position2u16
        vertex_count * 2 // GX_TexCoord2u8
    );

    disp_list.resize(disp_list_size);

    disp_list.write_into([&]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        write_text_vertices<U>(write_vertex, str, char_width, char_height);

        GX_End();
    });
}

void gfx::write_text_into_standard_display_list(gfx::display_list& disp_list, std::string_view str, u16 char_width, u16 char_height) {
    write_text_into_display_list<u8>([](u16 x, u16 y, u8 u, u8 v) {
        GX_Position2u16(x, y);
        GX_TexCoord2u8(u, v);
    }, disp_list, str, char_width, char_height);
}

template<typename U, typename F, typename P>
void gfx::write_text_vertices(F write_vertex, std::string_view str, P char_width, P char_height) {
    glm::vec<2, P, glm::defaultp> pos = { 0, 0 };

    for (auto ch : str) {
        glm::vec<2, P, glm::defaultp> pos_offset = { pos.x + char_width, pos.y + char_height };

        auto uv = get_2d_position_from_index<U>(ch);
        glm::vec<2, U, glm::defaultp> uv_offset = { uv.x + 1, uv.y + 1 };

        write_vertex(pos.x, pos.y, uv.x, uv.y);
        write_vertex(pos_offset.x, pos.y, uv_offset.x, uv.y);
        write_vertex(pos_offset.x, pos_offset.y, uv_offset.x, uv_offset.y);
        write_vertex(pos.x, pos_offset.y, uv.x, uv_offset.y);

        pos.x += char_width;
    }
}