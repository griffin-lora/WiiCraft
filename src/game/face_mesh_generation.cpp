#include "face_mesh_generation.hpp"
#include "chunk_mesh_generation.hpp"

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

template<typename Vf>
static void add_cube_front_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +x
    Vf::add_pos_vert(lo.x,lo.y, l.z);	// Top Left of the quad (bottom)
    Vf::add_uv_vert(u.x,u.y);
    
    Vf::add_pos_vert(lo.x,l.y, l.z);	// Top Right of the quad (bottom)
    Vf::add_uv_vert(u.x, uo.y);

    Vf::add_pos_vert(lo.x,l.y,lo.z);	// Bottom Right of the quad (bottom)
    Vf::add_uv_vert(uo.x,uo.y);

    Vf::add_pos_vert(lo.x,lo.y,lo.z);	// Bottom Left of the quad (bottom)
    Vf::add_uv_vert(uo.x, u.y);
}

template<typename Vf>
static void add_cube_back_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -x
    Vf::add_pos_vert( l.x, lo.y, l.z);	// Top Left of the quad (top)
    Vf::add_uv_vert(u.x,u.y);

    Vf::add_pos_vert(l.x, lo.y, lo.z);	// Top Right of the quad (top)
    Vf::add_uv_vert(uo.x,u.y);

    Vf::add_pos_vert(l.x, l.y, lo.z);	// Bottom Right of the quad (top)
    Vf::add_uv_vert(uo.x,uo.y);

    Vf::add_pos_vert(l.x, l.y, l.z);		// Bottom Left of the quad (top)
    Vf::add_uv_vert(u.x,uo.y);
    
}

template<typename Vf>
static void add_cube_top_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +y
    Vf::add_pos_vert(l.x,lo.y,lo.z);	// Bottom Left Of The Quad (Back)
    Vf::add_uv_vert(u.x,u.y);

    Vf::add_pos_vert(l.x,lo.y,l.z);	// Bottom Right Of The Quad (Back)
    Vf::add_uv_vert(uo.x,u.y);

    Vf::add_pos_vert(lo.x, lo.y,l.z);	// Top Right Of The Quad (Back)
    Vf::add_uv_vert(uo.x,uo.y);

    Vf::add_pos_vert(lo.x, lo.y,lo.z);	// Top Left Of The Quad (Back)
    Vf::add_uv_vert(u.x,uo.y);
    
}

template<typename Vf>
static void add_cube_bottom_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -y
    Vf::add_pos_vert(l.x, l.y, lo.z);		// Top Right Of The Quad (Front)
    Vf::add_uv_vert(u.x, u.y);

    Vf::add_pos_vert(lo.x, l.y, lo.z);	// Top Left Of The Quad (Front)
    Vf::add_uv_vert(uo.x, u.y);

    Vf::add_pos_vert(lo.x, l.y, l.z);	// Bottom Left Of The Quad (Front)
    Vf::add_uv_vert(uo.x, uo.y);

    Vf::add_pos_vert(l.x, l.y, l.z);	// Bottom Right Of The Quad (Front)
    Vf::add_uv_vert(u.x, uo.y);
}

template<typename Vf>
static void add_cube_right_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // +z
    Vf::add_pos_vert(lo.x, l.y,lo.z);	// Top Right Of The Quad (Right)
    Vf::add_uv_vert(u.x,uo.y);

    Vf::add_pos_vert(l.x, l.y, lo.z);		// Top Left Of The Quad (Right)
    Vf::add_uv_vert(uo.x,uo.y);

    Vf::add_pos_vert(l.x,lo.y, lo.z);	// Bottom Left Of The Quad (Right)
    Vf::add_uv_vert(uo.x,u.y);

    Vf::add_pos_vert(lo.x,lo.y,lo.z);	// Bottom Right Of The Quad (Right)
    Vf::add_uv_vert(u.x,u.y);
}

template<typename Vf>
static void add_cube_left_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo) {
    // -z
    Vf::add_pos_vert(lo.x, l.y, l.z);	// Top Right Of The Quad (Left)
    Vf::add_uv_vert(u.x,uo.y);

    Vf::add_pos_vert(lo.x, lo.y,l.z);	// Top Left Of The Quad (Left)
    Vf::add_uv_vert(u.x,u.y);

    Vf::add_pos_vert(l.x,lo.y,l.z);	// Bottom Left Of The Quad (Left)
    Vf::add_uv_vert(uo.x,u.y);

    Vf::add_pos_vert(l.x,l.y, l.z);	// Bottom Right Of The Quad (Left)
    Vf::add_uv_vert(uo.x,uo.y);
    
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

template void game::add_center_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// +x
template<typename Vf>
void game::add_front_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_front_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 0, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_front_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// -x
template<typename Vf>
void game::add_back_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_back_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 1, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_back_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// +y
template<typename Vf>
void game::add_top_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_top_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 0, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_top_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// -y
template<typename Vf>
void game::add_bottom_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_bottom_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_bottom_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// +z
template<typename Vf>
void game::add_right_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_right_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 4, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_right_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);

// -z
template<typename Vf>
void game::add_left_vertices(math::vector3u8 local_pos, block::type type) {
    auto& func = add_cube_left_vertices<Vf>;
    switch (type) {
        case block::type::DEBUG: add_cube_vertices<Vf>(func, { 5, 0 }, local_pos); return;
        case block::type::GRASS: add_cube_vertices<Vf>(func, { 3, 0 }, local_pos); return;
        case block::type::DIRT: add_cube_vertices<Vf>(func, { 2, 0 }, local_pos); return;
        default: return;
    }
}

template void game::add_left_vertices<chunk_mesh_vertex_functions>(math::vector3u8 local_pos, block::type type);