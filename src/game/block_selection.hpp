#pragma once
#include <optional>
#include "math.hpp"
#include "gfx/display_list.hpp"
#include "gfx/position_state.hpp"
#include "block_logic.hpp"

namespace game {
    struct block_selection {
        std::optional<math::vector3u8> last_block_pos;

	    gfx::display_list disp_list;
        gfx::position_state pos_state;
    };
    
    struct block_selection_vert_func {
        static inline void call(u8 x, u8 y, u8 z, u8, u8) {
            GX_Position3u8(x, y, z);
            GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
        }
    };

    void draw_block_selection(math::matrix view, const camera& cam, block_selection& bl_sel, std::optional<raycast>& raycast);
    
    void handle_raycast(math::matrix view, block_selection& block_selection, std::optional<raycast>& raycast);
};