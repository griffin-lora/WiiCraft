#pragma once
#include "logic.hpp"
#include "math.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include <optional>

namespace game {
    struct block_selection {
        std::optional<math::vector3u8> last_block_pos;

	    gfx::display_list standard_disp_list;
	    gfx::display_list foliage_disp_list;
        math::transform_3d tf;

        void update_if_needed(const math::matrix view, const camera& cam);
        void draw_first(const std::optional<raycast>& raycast) const;
        void draw_second(const std::optional<raycast>& raycast) const;
        void handle_raycast(const math::matrix view, ext::data_array<chunk::quad>& building_quads, const std::optional<raycast>& raycast);

        private:
            void update_mesh(const math::matrix view, ext::data_array<chunk::quad>& building_quads, const raycast& raycast);
    };
}