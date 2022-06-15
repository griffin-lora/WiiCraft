#include "face_mesh_generation.hpp"

using namespace game;

std::size_t game::get_center_vertex_count(block::type type) {
    return 0;
}

std::size_t game::get_any_face_vertex_count(block::type type) {
    switch (type) {
        default:
        case block::type::AIR:
            return 0;
        case block::type::DEBUG:
        case block::type::GRASS:
        case block::type::DIRT:
            return 4;
    }
}

static void add_cube_front_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +x
    *iters.pos_it++ = { lo.x,lo.y, l.z };	// Top Left of the quad (bottom)
    *iters.uv_it++ = { u.x,u.y };
    
    *iters.pos_it++ = { lo.x,l.y, l.z };	// Top Right of the quad (bottom)
    *iters.uv_it++ = { u.x, uo.y };

    *iters.pos_it++ = { lo.x,l.y,lo.z };	// Bottom Right of the quad (bottom)
    *iters.uv_it++ = { uo.x,uo.y };

    *iters.pos_it++ = { lo.x,lo.y,lo.z };	// Bottom Left of the quad (bottom)
    *iters.uv_it++ = { uo.x, u.y };
}

static void add_cube_back_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -x
    *iters.pos_it++ = {  l.x, lo.y, l.z };	// Top Left of the quad (top)
    *iters.uv_it++ = { u.x,u.y };

    *iters.pos_it++ = { l.x, lo.y, lo.z };	// Top Right of the quad (top)
    *iters.uv_it++ = { uo.x,u.y };

    *iters.pos_it++ = { l.x, l.y, lo.z };	// Bottom Right of the quad (top)
    *iters.uv_it++ = { uo.x,uo.y };

    *iters.pos_it++ = { l.x, l.y, l.z };		// Bottom Left of the quad (top)
    *iters.uv_it++ = { u.x,uo.y };
    
}

static void add_cube_top_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +y
    *iters.pos_it++ = { l.x,lo.y,lo.z };	// Bottom Left Of The Quad (Back)
    *iters.uv_it++ = { u.x,u.y };

    *iters.pos_it++ = { l.x,lo.y,l.z };	// Bottom Right Of The Quad (Back)
    *iters.uv_it++ = { uo.x,u.y };

    *iters.pos_it++ = { lo.x, lo.y,l.z };	// Top Right Of The Quad (Back)
    *iters.uv_it++ = { uo.x,uo.y };

    *iters.pos_it++ = { lo.x, lo.y,lo.z };	// Top Left Of The Quad (Back)
    *iters.uv_it++ = { u.x,uo.y };
    
}

static void add_cube_bottom_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -y
    *iters.pos_it++ = { l.x, l.y, lo.z };		// Top Right Of The Quad (Front)
    *iters.uv_it++ = { u.x, u.y };

    *iters.pos_it++ = { lo.x, l.y, lo.z };	// Top Left Of The Quad (Front)
    *iters.uv_it++ = { uo.x, u.y };

    *iters.pos_it++ = { lo.x, l.y, l.z };	// Bottom Left Of The Quad (Front)
    *iters.uv_it++ = { uo.x, uo.y };

    *iters.pos_it++ = { l.x, l.y, l.z };	// Bottom Right Of The Quad (Front)
    *iters.uv_it++ = { u.x, uo.y };
}

static void add_cube_right_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +z
    *iters.pos_it++ = {  lo.x, l.y,lo.z };	// Top Right Of The Quad (Right)
    *iters.uv_it++ = { u.x,uo.y };

    *iters.pos_it++ = { l.x, l.y, lo.z };		// Top Left Of The Quad (Right)
    *iters.uv_it++ = { uo.x,uo.y };

    *iters.pos_it++ = { l.x,lo.y, lo.z };	// Bottom Left Of The Quad (Right)
    *iters.uv_it++ = { uo.x,u.y };

    *iters.pos_it++ = { lo.x,lo.y,lo.z };	// Bottom Right Of The Quad (Right)
    *iters.uv_it++ = { u.x,u.y };
}

static void add_cube_left_vertices(ms_iters& iters, math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -z
    *iters.pos_it++ = { lo.x, l.y, l.z };	// Top Right Of The Quad (Left)
    *iters.uv_it++ = { u.x,uo.y };

    *iters.pos_it++ = { lo.x, lo.y,l.z };	// Top Left Of The Quad (Left)
    *iters.uv_it++ = { u.x,u.y };

    *iters.pos_it++ = { l.x,lo.y,l.z };	// Bottom Left Of The Quad (Left)
    *iters.uv_it++ = { uo.x,u.y };

    *iters.pos_it++ = { l.x,l.y, l.z };	// Bottom Right Of The Quad (Left)
    *iters.uv_it++ = { uo.x,uo.y };
    
}

static inline math::vector3u8 get_local_pos_offset(math::vector3u8 local_pos) {
    return { local_pos.x + 1, local_pos.y + 1, local_pos.z + 1 };
}

static inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos) {
    return { uv_pos.x + 1, uv_pos.y + 1 };
}

template<typename F>
static inline void add_cube_vertices(F func, math::vector2u8 uv_pos, ms_iters& iters, math::vector3u8 local_pos) {
    func(iters, local_pos, get_local_pos_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos));
}

void game::add_center_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    // no
}

// +x
void game::add_front_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_front_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 0, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 3, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}

// -x
void game::add_back_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_back_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 1, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 3, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}

// +y
void game::add_top_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_top_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 0, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}

// -y
void game::add_bottom_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_bottom_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 3, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}

// +z
void game::add_right_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_right_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 4, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 3, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}

// -z
void game::add_left_vertices(ms_iters& iters, math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_left_vertices;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices(func, { 5, 0 }, iters, local_pos); return;
        case block::type::GRASS: add_cube_vertices(func, { 3, 0 }, iters, local_pos); return;
        case block::type::DIRT: add_cube_vertices(func, { 2, 0 }, iters, local_pos); return;
        default: return;
    }
}