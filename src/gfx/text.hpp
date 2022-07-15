#pragma once
#include "gfx/display_list.hpp"
#include "math.hpp"
#include <string_view>

namespace gfx {
    template<typename O>
    inline glm::vec<2, O, glm::defaultp> get_2d_position_from_index(u8 index) {
        return { (O)index % 16, (O)index / 16 };
    }

    void write_text_into_display_list(gfx::display_list& disp_list, std::string_view str, u16 char_width, u16 char_height);

    template<typename P>
    constexpr P get_text_width(std::string_view str, P char_width) {
        return str.size() * char_width;
    }

    template<typename U, typename F, typename P>
    void write_text_vertices(F write_vertex, std::string_view str, P char_width, P char_height);
}