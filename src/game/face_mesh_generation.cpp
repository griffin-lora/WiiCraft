#include "face_mesh_generation.hpp"
#include "chunk_mesh_generation.hpp"
#include "block_selection_mesh_generation.hpp"

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

template void game::add_center_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_center_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_front_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_front_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_back_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_back_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_top_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_top_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_bottom_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_bottom_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_right_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_right_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

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

template void game::add_left_vertices<chunk_mesh_vert_func>(math::vector3u8 local_pos, block::type type);
template void game::add_left_vertices<block_selection_vert_func>(math::vector3u8 local_pos, block::type type);

std::size_t game::get_block_vertex_count(block::type type) {
    return
        get_face_vertex_count<block::face::FRONT>(type) +
        get_face_vertex_count<block::face::BACK>(type) +
        get_face_vertex_count<block::face::TOP>(type) +
        get_face_vertex_count<block::face::BOTTOM>(type) +
        get_face_vertex_count<block::face::RIGHT>(type) +
        get_face_vertex_count<block::face::LEFT>(type);
}