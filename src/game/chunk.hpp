#pragma once
#include "block.hpp"
#include "common.hpp"
#include "math.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"
#include "math/transform_3d.hpp"
#include <unordered_map>
#include <optional>
#include <functional>

namespace game {
    struct chunk {
        using map = std::unordered_map<math::vector3s32, chunk, hash_vector3s32>;
        using opt_ref = std::optional<std::reference_wrapper<chunk>>;
        using const_opt_ref = std::optional<std::reference_wrapper<const chunk>>;

        static constexpr s32 SIZE = 32;
        static constexpr u32 BLOCKS_COUNT = SIZE * SIZE * SIZE;
        static constexpr std::size_t MAX_VERTEX_COUNT = 0xefff;

        struct vertex {
            enum class type : u8 {
                STANDARD,
                FOLIAGE
            };
            type tp;
            math::vector3u8 pos;
            math::vector2u8 uv;
        };

        gfx::display_list disp_list;
        bool update_mesh_important = false;
        bool update_mesh_unimportant = false;
        struct neighborhood {
            opt_ref front;
            opt_ref back;
            opt_ref top;
            opt_ref bottom;
            opt_ref right;
            opt_ref left;
        };

        neighborhood nh;
        bool update_neighborhood = false;
        ext::data_array<block> blocks;

        math::transform_3d tf;

        bool modified = false;
        
        inline chunk(const math::matrix view, const math::vector3s32& pos) {
            tf.set_position(view, pos.x * chunk::SIZE, pos.y * chunk::SIZE, pos.z * chunk::SIZE);
        }
    };
}