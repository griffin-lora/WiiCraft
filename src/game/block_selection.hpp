#pragma once
#include "block_raycast.hpp"
#include "logic.hpp"
#include "math.hpp"
#include "block_mesh_generation.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include <optional>

namespace game {
    struct block_selection {
        static constexpr u32 MAT = GX_PNMTX4;

        std::optional<math::vector3s32> last_block_pos;
        std::optional<block> last_block;

	    gfx::display_list standard_disp_list;
	    gfx::display_list foliage_disp_list;
	    gfx::display_list water_disp_list;
        math::transform_3d tf;

        inline block_selection() { }

        void update_if_needed(const math::matrix view, const camera& cam);
        void draw(const std::optional<block_raycast>& raycast) const;
        void handle_raycast(const math::matrix view, block_quad_building_arrays& building_arrays, const std::optional<block_raycast>& raycast);

        private:
            void update_mesh(const math::matrix view, block_quad_building_arrays& building_arrays, const block_raycast& raycast);
    };
}