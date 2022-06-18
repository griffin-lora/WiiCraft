#pragma once
#include "gfx.hpp"
#include "gfx/display_list.hpp"
#include "math.hpp"

namespace game {
    struct cursor {
        glm::vec2 pos;

        gfx::display_list disp_list;
    };

    void init(cursor& cursor);

    void draw_cursor(gfx::texture& texture, math::matrix model_view, cursor& cursor);
};