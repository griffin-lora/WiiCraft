#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "util.hpp"
#include <array>

namespace game {
    #define BF_FUNC static constexpr

    using bl_st = const block::state&;

    template<block::type type>
    struct block_functionality {
        BF_FUNC block_traits get_block_traits(bl_st) { return {}; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {}; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block&) { return false; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf&, math::vector3u8, bl_st) {}

        BF_FUNC std::array<math::box, 0> get_boxes(bl_st) { return {}; }
    };
}