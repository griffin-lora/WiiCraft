#include "game.hpp"

using namespace game;

std::size_t game::get_center_vertex_count(block::type type) {
    return 0;
}

std::size_t game::get_any_face_vertex_count(block::type type) {
    switch (type) {
        default:
        case block::type::AIR: {
            return 0;
        }
        case block::type::GRASS: {
            return 4;
        }
    }
}

inline void add_cube_front_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // +x
    *it++ = {
        { lo.x,lo.y, l.z },	// Top Left of the quad (bottom)
        { u.x,u.y }
    };
    *it++ = {
        { lo.x,l.y, l.z },	// Top Right of the quad (bottom)
        { uo.x,u.y }
    };
    *it++ = {
        { lo.x,l.y,lo.z },	// Bottom Right of the quad (bottom)
        { uo.x,uo.y }
    };
    *it++ = {
        { lo.x,lo.y,lo.z },	// Bottom Left of the quad (bottom)
        { u.x,uo.y }
    };
}

inline void add_cube_back_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // -x
    *it++ = {
        {  l.x, lo.y, l.z },	// Top Left of the quad (top)
        { u.x,u.y }
    };
    *it++ = {
        { l.x, lo.y, lo.z },	// Top Right of the quad (top)
        { uo.x,u.y }
    };
    *it++ = {
        { l.x, l.y, lo.z },	// Bottom Right of the quad (top)
        { uo.x,uo.y }
    };
    *it++ = {
        { l.x, l.y, l.z },		// Bottom Left of the quad (top)
        { u.x,uo.y }
    };
}

inline void add_cube_left_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // +z
    *it++ = {
        {  lo.x, l.y,lo.z },	// Top Right Of The Quad (Right)
        { u.x,u.y }
    };
    *it++ = {
        { l.x, l.y, lo.z },		// Top Left Of The Quad (Right)
        { uo.x,u.y }
    };
    *it++ = {
        { l.x,lo.y, lo.z },	// Bottom Left Of The Quad (Right)
        { uo.x,uo.y }
    };
    *it++ = {
        { lo.x,lo.y,lo.z },	// Bottom Right Of The Quad (Right)
        { u.x,uo.y }
    };
}

inline void add_cube_right_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // -z
    *it++ = {
        { lo.x, l.y, l.z },	// Top Right Of The Quad (Left)
        { u.x,u.y }
    };
    *it++ = {
        { lo.x, lo.y,l.z },	// Top Left Of The Quad (Left)
        { uo.x,u.y }
    };
    *it++ = {
        { l.x,lo.y,l.z },	// Bottom Left Of The Quad (Left)
        { uo.x,uo.y }
    };
    *it++ = {
        { l.x,l.y, l.z },	// Bottom Right Of The Quad (Left)
        { u.x,uo.y }
    };
}

inline void add_cube_top_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // +y
   *it++ = {
        { l.x,lo.y,lo.z },	// Bottom Left Of The Quad (Back)
        { u.x,u.y }
    };
    *it++ = {
        { l.x,lo.y,l.z },	// Bottom Right Of The Quad (Back)
        { uo.x,u.y }
    };
    *it++ = {
        { lo.x, lo.y,l.z },	// Top Right Of The Quad (Back)
        { uo.x,uo.y }
    };
    *it++ = {
        { lo.x, lo.y,lo.z },	// Top Left Of The Quad (Back)
        { u.x,uo.y }
    };
}

inline void add_cube_bottom_vertices(math::vector3u8 l, math::vector3u8 lo, math::vector2u8 u, math::vector2u8 uo, vertex_it& it) {
    // -y
    *it++ = {
        { l.x, l.y, lo.z },		// Top Right Of The Quad (Front)
        { u.x, u.y }
    };
    *it++ = {
        { lo.x, l.y, lo.z },	// Top Left Of The Quad (Front)
        { uo.x, u.y }
    };
    *it++ = {
        { lo.x, l.y, l.z },	// Bottom Left Of The Quad (Front)
        { uo.x, uo.y }
    };
    *it++ = {
        { l.x, l.y, l.z },	// Bottom Right Of The Quad (Front)
        { u.x, uo.y }
    };
}

inline math::vector3u8 get_local_position_offset(math::vector3u8 local_pos) {
    return { local_pos.x + 1, local_pos.y + 1, local_pos.z + 1 };
}

inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos) {
    return { uv_pos.x + 1, uv_pos.y + 1 };
}

void game::add_center_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    // no
}

// +x
void game::add_front_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 0, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 3, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_front_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}

// -x
void game::add_back_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 1, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 3, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_back_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}

// +z
void game::add_left_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 2, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 3, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_left_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}

// -z
void game::add_right_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 3, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 3, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_right_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}

// +y
void game::add_top_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 4, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 0, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_top_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}

// -y
void game::add_bottom_vertices(math::vector3u8 local_pos, vertex_it& it, block::type type) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        default: {
            math::vector2u8 uv_pos;
            switch (type) {
                case block::type::DEBUG: {
                    uv_pos = { 5, 0 };
                    break;
                }
                case block::type::GRASS: {
                    uv_pos = { 2, 0 };
                    break;
                }
                default: {
                    return;
                }
            }
            add_cube_bottom_vertices(local_pos, get_local_position_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos), it);
        }
    }
}