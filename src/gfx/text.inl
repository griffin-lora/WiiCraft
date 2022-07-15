#pragma once
#include "text.hpp"

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