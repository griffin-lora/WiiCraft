#pragma once
#include <unordered_map>
#include <optional>
#include <functional>
#include "math.hpp"
#include "ext/data_array.hpp"
#include "block.hpp"

namespace game {
    struct chunk {
        using map = std::unordered_map<math::vector3s32, chunk>;
        using opt_ref = std::optional<std::reference_wrapper<chunk>>;
        using const_opt_ref = std::optional<std::reference_wrapper<const chunk>>;

        static constexpr s32 SIZE = 32;
        static constexpr u32 BLOCKS_COUNT = SIZE * SIZE * SIZE;
        
        struct mesh {
            struct vertex {
                math::vector3u8 local_position;
                math::vector2u8 uv_position;
            };
            ext::data_array<vertex> vertices;
        };

        mesh ms;
        bool update_mesh = true;

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
        ext::data_array<block> blocks = { BLOCKS_COUNT };

        math::matrix model;
        math::matrix model_view;
    };
};