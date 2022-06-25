#pragma once
#include "face_mesh_generation_core.hpp"

template<typename Vf>
void game::add_flat_front_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +x
    vf(lo.x,lo.y, l.z, u.x,u.y);	// Top Left of the quad (bottom)
    
    vf(lo.x,l.y, l.z, u.x, uo.y);	// Top Right of the quad (bottom)

    vf(lo.x,l.y,lo.z, uo.x,uo.y);	// Bottom Right of the quad (bottom)

    vf(lo.x,lo.y,lo.z, uo.x, u.y);	// Bottom Left of the quad (bottom)
}

template<typename Vf>
void game::add_flat_back_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -x
    vf( l.x, lo.y, l.z, u.x,u.y);	// Top Left of the quad (top)

    vf(l.x, lo.y, lo.z, uo.x,u.y);	// Top Right of the quad (top)

    vf(l.x, l.y, lo.z, uo.x,uo.y);	// Bottom Right of the quad (top)

    vf(l.x, l.y, l.z, u.x,uo.y);		// Bottom Left of the quad (top)
    
}

template<typename Vf>
void game::add_flat_top_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +y
    vf(l.x,lo.y,lo.z, u.x,u.y);	// Bottom Left Of The Quad (Back)

    vf(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Right Of The Quad (Back)

    vf(lo.x, lo.y,l.z, uo.x,uo.y);	// Top Right Of The Quad (Back)

    vf(lo.x, lo.y,lo.z, u.x,uo.y);	// Top Left Of The Quad (Back)
    
}

template<typename Vf>
void game::add_flat_bottom_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -y
    vf(l.x, l.y, lo.z, u.x, u.y);		// Top Right Of The Quad (Front)

    vf(lo.x, l.y, lo.z, uo.x, u.y);	// Top Left Of The Quad (Front)

    vf(lo.x, l.y, l.z, uo.x, uo.y);	// Bottom Left Of The Quad (Front)

    vf(l.x, l.y, l.z, u.x, uo.y);	// Bottom Right Of The Quad (Front)
}

template<typename Vf>
void game::add_flat_right_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +z
    vf(lo.x, l.y,lo.z, u.x,uo.y);	// Top Right Of The Quad (Right)

    vf(l.x, l.y, lo.z, uo.x,uo.y);		// Top Left Of The Quad (Right)

    vf(l.x,lo.y, lo.z, uo.x,u.y);	// Bottom Left Of The Quad (Right)

    vf(lo.x,lo.y,lo.z, u.x,u.y);	// Bottom Right Of The Quad (Right)
}

template<typename Vf>
void game::add_flat_left_vertices(Vf& vf, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -z
    vf(lo.x, l.y, l.z, u.x,uo.y);	// Top Right Of The Quad (Left)

    vf(lo.x, lo.y,l.z, u.x,u.y);	// Top Left Of The Quad (Left)

    vf(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Left Of The Quad (Left)

    vf(l.x,l.y, l.z, uo.x,uo.y);	// Bottom Right Of The Quad (Left)
    
}