#pragma once
#include <optional>
#include "math.hpp"
#include "gfx/display_list.hpp"
#include "gfx/position_state.hpp"

namespace game {
    struct block_selection {
        std::optional<math::vector3u8> last_block_pos;

	    gfx::display_list disp_list;
        gfx::position_state pos_state;
    };
};