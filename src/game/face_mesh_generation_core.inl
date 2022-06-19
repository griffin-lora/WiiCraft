#pragma once
#include "face_mesh_generation_core.hpp"

template<typename Vf>
void game::add_cube_front_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +x
    Vf::call(lo.x,lo.y, l.z, u.x,u.y);	// Top Left of the quad (bottom)
    
    Vf::call(lo.x,l.y, l.z, u.x, uo.y);	// Top Right of the quad (bottom)

    Vf::call(lo.x,l.y,lo.z, uo.x,uo.y);	// Bottom Right of the quad (bottom)

    Vf::call(lo.x,lo.y,lo.z, uo.x, u.y);	// Bottom Left of the quad (bottom)
}

template<typename Vf>
void game::add_cube_back_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -x
    Vf::call( l.x, lo.y, l.z, u.x,u.y);	// Top Left of the quad (top)

    Vf::call(l.x, lo.y, lo.z, uo.x,u.y);	// Top Right of the quad (top)

    Vf::call(l.x, l.y, lo.z, uo.x,uo.y);	// Bottom Right of the quad (top)

    Vf::call(l.x, l.y, l.z, u.x,uo.y);		// Bottom Left of the quad (top)
    
}

template<typename Vf>
void game::add_cube_top_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +y
    Vf::call(l.x,lo.y,lo.z, u.x,u.y);	// Bottom Left Of The Quad (Back)

    Vf::call(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Right Of The Quad (Back)

    Vf::call(lo.x, lo.y,l.z, uo.x,uo.y);	// Top Right Of The Quad (Back)

    Vf::call(lo.x, lo.y,lo.z, u.x,uo.y);	// Top Left Of The Quad (Back)
    
}

template<typename Vf>
void game::add_cube_bottom_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -y
    Vf::call(l.x, l.y, lo.z, u.x, u.y);		// Top Right Of The Quad (Front)

    Vf::call(lo.x, l.y, lo.z, uo.x, u.y);	// Top Left Of The Quad (Front)

    Vf::call(lo.x, l.y, l.z, uo.x, uo.y);	// Bottom Left Of The Quad (Front)

    Vf::call(l.x, l.y, l.z, u.x, uo.y);	// Bottom Right Of The Quad (Front)
}

template<typename Vf>
void game::add_cube_right_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +z
    Vf::call(lo.x, l.y,lo.z, u.x,uo.y);	// Top Right Of The Quad (Right)

    Vf::call(l.x, l.y, lo.z, uo.x,uo.y);		// Top Left Of The Quad (Right)

    Vf::call(l.x,lo.y, lo.z, uo.x,u.y);	// Bottom Left Of The Quad (Right)

    Vf::call(lo.x,lo.y,lo.z, u.x,u.y);	// Bottom Right Of The Quad (Right)
}

template<typename Vf>
void game::add_cube_left_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -z
    Vf::call(lo.x, l.y, l.z, u.x,uo.y);	// Top Right Of The Quad (Left)

    Vf::call(lo.x, lo.y,l.z, u.x,u.y);	// Top Left Of The Quad (Left)

    Vf::call(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Left Of The Quad (Left)

    Vf::call(l.x,l.y, l.z, uo.x,uo.y);	// Bottom Right Of The Quad (Left)
    
}