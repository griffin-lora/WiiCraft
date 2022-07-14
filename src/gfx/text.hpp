#pragma once
#include "gfx/display_list.hpp"
#include "math/transform_2d.hpp"
#include <string_view>

namespace gfx {
    inline math::vector2u8 get_2d_position_from_index(u8 index) {
        return { index % 16, index / 16 };
    }

    struct text {
        math::transform_2d tf;
        display_list disp_list;
        
        text() = default;
        inline text(std::string_view str, u16 char_width, u16 char_height) {
            update(str, char_width, char_height);
        }

        void update(std::string_view str, u16 char_width, u16 char_height);

        void draw();
    };
}