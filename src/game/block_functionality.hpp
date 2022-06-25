#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "util.hpp"
#include <array>

namespace game {
    constexpr u8 block_draw_size = 4;
    constexpr u8 half_block_draw_size = 2;

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

    struct draw_positions {
        math::vector3u8 block_draw_pos;
        math::vector2u8 uv_draw_pos;
    };

    template<typename Bf>
    struct flat_face_block_functionality {
        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            // Get information about where the vertices will be positioned
            draw_positions d_positions = {
                .block_draw_pos = block_pos * block_draw_size,
                .uv_draw_pos = Bf::template get_uv_position<face>(st) * block_draw_size
            };
            draw_positions offset_d_positions = Bf::get_offset_draw_positions(d_positions, st);
            add_flat_face_vertices<face>(
                vf,
                d_positions.block_draw_pos,
                offset_d_positions.block_draw_pos,
                d_positions.uv_draw_pos,
                offset_d_positions.uv_draw_pos
            );
        }
    };

    template<typename Bf>
    struct cube_block_functionality : public flat_face_block_functionality<cube_block_functionality<Bf>> {
        BF_FUNC block_traits get_block_traits(bl_st) { return {
            .visible = true,
            .general_vertex_count = 0
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {
            .partially_transparent = false,
            .vertex_count = 4
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block& block) { return get_neighbor_face_traits<face>(block).partially_transparent; }

        BF_FUNC std::array<math::box, 1> get_boxes(bl_st) {
            return {
                math::box{
                    .lesser_corner = { 0.0f, 0.0f, 0.0f },
                    .greater_corner = { 1.0f, 1.0f, 1.0f }
                }
            };
        }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st st) {
            return Bf::template get_uv_position<face>(st);
        }

        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& draw_positions, bl_st) {
            return {
                .block_draw_pos = draw_positions.block_draw_pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size },
                .uv_draw_pos = draw_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size }
            };
        }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{0, 0}; },
                []() { return v2u8{1, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{4, 0}; },
                []() { return v2u8{5, 0}; }
            );
        }
    };
}