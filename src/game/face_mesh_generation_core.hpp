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
}