#pragma once
#include "block.hpp"
#include "common.hpp"
#include "math.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list_new.hpp"
#include "math/transform_3d.hpp"
#include "chrono.hpp"
#include "pool.hpp"
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

        static constexpr u32 mat = GX_PNMTX5;

        static constexpr s32 size = 16;
        static constexpr u32 blocks_count = size * size * size;
        
        struct neighborhood {
            opt_ref front;
            opt_ref back;
            opt_ref top;
            opt_ref bottom;
            opt_ref right;
            opt_ref left;
        };

        size_t blocks_chunk_index;
        block_type_t* blocks;
        neighborhood nh;

        u8 alpha = 0;
        // TODO: this is stupid
        std::vector<pool_display_list_t> solid_display_lists;
        std::vector<pool_display_list_t> transparent_display_lists;
        std::vector<pool_display_list_t> transparent_double_sided_display_lists;

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

        static constexpr chrono::us fade_time = 1500000;

        enum class fade_state {
            none,
            in,
            out
        };

        fade_state fade_st = fade_state::none;
        chrono::us fade_start;
        
        inline chunk(const math::matrix view, const math::vector3s32& pos) {
            tf.set_position(view, pos.x * chunk::size, pos.y * chunk::size, pos.z * chunk::size);
        }
    };
}