#pragma once
#include "math.hpp"

namespace game {
    template<typename Vf>
    void add_cube_front_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_cube_back_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_cube_top_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_cube_bottom_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_cube_right_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
    template<typename Vf>
    void add_cube_left_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo);
};