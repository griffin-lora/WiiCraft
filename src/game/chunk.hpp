#pragma once
#include <unordered_map>
#include <optional>
#include <functional>
#include "math.hpp"
#include "ext/data_array.hpp"
#include "block.hpp"
#include "gfx/display_list.hpp"

namespace game {
    struct chunk {
        using map = std::unordered_map<math::vector3s32, chunk>;
        using opt_ref = std::optional<std::reference_wrapper<chunk>>;
        using const_opt_ref = std::optional<std::reference_wrapper<const chunk>>;

        static constexpr s32 SIZE = 32;
        static constexpr u32 BLOCKS_COUNT = SIZE * SIZE * SIZE;
        
        struct mesh {
            using pos_vertex = math::vector3u8;
            using uv_vertex = math::vector2u8;
            ext::data_array<pos_vertex> pos_vertices;
            ext::data_array<uv_vertex> uv_vertices;

            struct iterators {
                ext::data_array<pos_vertex>::iterator pos_it;
                ext::data_array<uv_vertex>::iterator uv_it;
            };
        };

        gfx::display_list disp_list;
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
        ext::data_array<block> blocks;

        math::matrix model;
        math::matrix model_view;

        bool modified = false;
    };
};