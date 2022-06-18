#pragma once
#include <optional>
#include "math.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include "logic.hpp"

namespace game {
    struct block_selection {
        std::optional<math::vector3u8> last_block_pos;

	    gfx::display_list disp_list;
        math::transform_3d tf;
    };

    void draw_block_selection(math::matrix view, const camera& cam, block_selection& bl_sel, std::optional<raycast>& raycast);
    
    void handle_raycast(math::matrix view, block_selection& block_selection, std::optional<raycast>& raycast);
};