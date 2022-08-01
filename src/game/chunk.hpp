#pragma once
#include "block.hpp"
#include "block_mesh_generation.hpp"
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

        using display_lists = block_mesh_layers<single_type_container<gfx::display_list>>;

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