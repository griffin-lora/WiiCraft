#pragma once
#include "gfx.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_2d.hpp"
#include "math.hpp"
#include <optional>

namespace game {
    struct cursor {
        static constexpr u32 MAT = GX_PNMTX3;

        math::transform_2d tf;
        gfx::display_list disp_list;

        cursor();

        void update_from_pointer_position(u16 v_width, u16 v_height, const std::optional<glm::vec2>& pointer_pos);

        void draw() const;
    };
}