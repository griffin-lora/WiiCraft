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

        void update_if_needed(const math::matrix view, const camera& cam);
        void draw(const std::optional<raycast>& raycast) const;
        void handle_raycast(const math::matrix view, const std::optional<raycast>& raycast);

        private:
            void update_mesh(const math::matrix view, const math::vector3s32& ch_pos, math::vector3u8 bl_pos, block::type type);
    };
};