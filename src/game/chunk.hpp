#pragma once
#include "block.hpp"
#include "common.hpp"
#include "math.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include "chrono.hpp"
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>

namespace game {
    struct chunk {
        using map = std::unordered_map<math::vector3s32, chunk, hash_vector3s32>;
        using opt_ref = std::optional<std::reference_wrapper<chunk>>;
        using const_opt_ref = std::optional<std::reference_wrapper<const chunk>>;

        using pos_set = std::unordered_set<math::vector3s32, hash_vector3s32>;

        static constexpr u32 MAT = GX_PNMTX5;

        static constexpr s32 SIZE = 32;
        static constexpr u32 BLOCKS_COUNT = SIZE * SIZE * SIZE;
        
        static constexpr s32 MAX_STANDARD_QUAD_COUNT = 0x3eff;
        static constexpr s32 MAX_FOLIAGE_QUAD_COUNT = 0x1000;
        static constexpr s32 MAX_TRANSPARENT_QUAD_COUNT = 0x1000;

        struct vertex {
            math::vector3u8 pos;
            math::vector2u8 uv;
        };

        struct quad {
            vertex vert0;
            vertex vert1;
            vertex vert2;
            vertex vert3;
        };
        
        struct neighborhood {
            opt_ref front;
            opt_ref back;
            opt_ref top;
            opt_ref bottom;
            opt_ref right;
            opt_ref left;
        };

        ext::data_array<block> blocks;
        neighborhood nh;

        struct display_lists {
            gfx::display_list standard;
            gfx::display_list foliage;
            gfx::display_list transparent;
        };

        u8 alpha = 0;
        display_lists core_disp_lists;
        display_lists shell_disp_lists;

        math::transform_3d tf;

        std::size_t invisible_block_count = 0;
        std::size_t fully_opaque_block_count = 0;
        std::size_t partially_opaque_block_count = 0;

        bool update_core_mesh_important = false;
        bool update_core_mesh_unimportant = false;

        bool update_shell_mesh_important = false;
        bool update_shell_mesh_unimportant = false;

        bool update_neighborhood = false;

        bool should_erase = false;

        bool modified = false;
        
        bool fade_in_when_mesh_is_updated = false;

        static constexpr chrono::us FADE_TIME = 1500000;

        enum class fade_state {
            NONE,
            IN,
            OUT
        };

        fade_state fade_st = fade_state::NONE;
        chrono::us fade_start;
        
        inline chunk(const math::matrix view, const math::vector3s32& pos) {
            tf.set_position(view, pos.x * chunk::SIZE, pos.y * chunk::SIZE, pos.z * chunk::SIZE);
        }
    };
}