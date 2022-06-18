#pragma once
#include "face_mesh_generation.hpp"

template<typename Vf>
static void add_cube_front_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +x
    Vf::call(lo.x,lo.y, l.z, u.x,u.y);	// Top Left of the quad (bottom)
    
    Vf::call(lo.x,l.y, l.z, u.x, uo.y);	// Top Right of the quad (bottom)

    Vf::call(lo.x,l.y,lo.z, uo.x,uo.y);	// Bottom Right of the quad (bottom)

    Vf::call(lo.x,lo.y,lo.z, uo.x, u.y);	// Bottom Left of the quad (bottom)
}

template<typename Vf>
static void add_cube_back_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -x
    Vf::call( l.x, lo.y, l.z, u.x,u.y);	// Top Left of the quad (top)

    Vf::call(l.x, lo.y, lo.z, uo.x,u.y);	// Top Right of the quad (top)

    Vf::call(l.x, l.y, lo.z, uo.x,uo.y);	// Bottom Right of the quad (top)

    Vf::call(l.x, l.y, l.z, u.x,uo.y);		// Bottom Left of the quad (top)
    
}

template<typename Vf>
static void add_cube_top_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +y
    Vf::call(l.x,lo.y,lo.z, u.x,u.y);	// Bottom Left Of The Quad (Back)

    Vf::call(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Right Of The Quad (Back)

    Vf::call(lo.x, lo.y,l.z, uo.x,uo.y);	// Top Right Of The Quad (Back)

    Vf::call(lo.x, lo.y,lo.z, u.x,uo.y);	// Top Left Of The Quad (Back)
    
}

template<typename Vf>
static void add_cube_bottom_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -y
    Vf::call(l.x, l.y, lo.z, u.x, u.y);		// Top Right Of The Quad (Front)

    Vf::call(lo.x, l.y, lo.z, uo.x, u.y);	// Top Left Of The Quad (Front)

    Vf::call(lo.x, l.y, l.z, uo.x, uo.y);	// Bottom Left Of The Quad (Front)

    Vf::call(l.x, l.y, l.z, u.x, uo.y);	// Bottom Right Of The Quad (Front)
}

template<typename Vf>
static void add_cube_right_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +z
    Vf::call(lo.x, l.y,lo.z, u.x,uo.y);	// Top Right Of The Quad (Right)

    Vf::call(l.x, l.y, lo.z, uo.x,uo.y);		// Top Left Of The Quad (Right)

    Vf::call(l.x,lo.y, lo.z, uo.x,u.y);	// Bottom Left Of The Quad (Right)

    Vf::call(lo.x,lo.y,lo.z, u.x,u.y);	// Bottom Right Of The Quad (Right)
}

template<typename Vf>
static void add_cube_left_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -z
    Vf::call(lo.x, l.y, l.z, u.x,uo.y);	// Top Right Of The Quad (Left)

    Vf::call(lo.x, lo.y,l.z, u.x,u.y);	// Top Left Of The Quad (Left)

    Vf::call(l.x,lo.y,l.z, uo.x,u.y);	// Bottom Left Of The Quad (Left)

    Vf::call(l.x,l.y, l.z, uo.x,uo.y);	// Bottom Right Of The Quad (Left)
    
}

static inline math::vector3u8 get_local_pos_offset(math::vector3u8 local_pos) {
    return { local_pos.x + 1, local_pos.y + 1, local_pos.z + 1 };
}

static inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos) {
    return { uv_pos.x + 1, uv_pos.y + 1 };
}

template<typename Vf, typename F>
static inline void add_cube_vertices(F func, math::vector2u8 uv_pos, math::vector3u8 local_pos) {
    func(local_pos, get_local_pos_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos));
}

template<typename Vf>
void game::add_center_vertices(math::vector3u8 local_pos, block::type type) {
    // no
}

// +x
template<typename Vf>
void game::add_front_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_front_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 0, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}

// -x
template<typename Vf>
void game::add_back_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_back_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 1, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}

// +y
template<typename Vf>
void game::add_top_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_top_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 0, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}

// -y
template<typename Vf>
void game::add_bottom_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_bottom_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}

// +z
template<typename Vf>
void game::add_right_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_right_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}

// -z
template<typename Vf>
void game::add_left_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_left_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 5, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::WOOD_PLANKS: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        default: return;
    }
}
