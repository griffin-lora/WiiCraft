#pragma once
#include "math.hpp"
#include "util.hpp"

namespace game {
    template<typename Vf>
    void add_flat_front_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_flat_back_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_flat_top_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_flat_bottom_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_flat_right_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_flat_left_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);

    template<block::face face, typename Vf>
    constexpr void add_flat_face_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
        call_face_func_for<face, void>(
            add_flat_front_vertices<Vf>,
            add_flat_back_vertices<Vf>,
            add_flat_top_vertices<Vf>,
            add_flat_bottom_vertices<Vf>,
            add_flat_right_vertices<Vf>,
            add_flat_left_vertices<Vf>,
            vf, l, lo, u, uo
        );
    }

    struct draw_positions {
        math::vector3u8 block_draw_pos;
        math::vector2u8 uv_draw_pos;
    };

    template<block::face face, typename Bf, typename Vf>
    constexpr void add_flat_face_vertices_from_block_position(Vf& vf, math::vector3u8 block_pos, bl_st st) {
        // Get information about where the vertices will be positioned
        draw_positions d_positions = {
            .block_draw_pos = block_pos * block_draw_size,
            .uv_draw_pos = Bf::template get_uv_position<face>(st) * block_draw_size
        };
        d_positions = Bf::get_draw_positions(d_positions, st);
        draw_positions offset_d_positions = Bf::get_offset_draw_positions(d_positions, st);
        add_flat_face_vertices<face>(
            vf,
            d_positions.block_draw_pos,
            offset_d_positions.block_draw_pos,
            d_positions.uv_draw_pos,
            offset_d_positions.uv_draw_pos
        );
    }
}