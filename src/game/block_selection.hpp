#pragma once
#include <optional>
#include "math.hpp"
#include "gfx/display_list.hpp"

namespace game {
    struct block_selection_state {
        math::vector3s32 chunk_pos = {0, 0, 0};
        std::optional<math::vector3u8> last_block_pos;
        math::vector3u8 block_pos = {0, 0, 0};
	    gfx::display_list disp_list;
    };
};