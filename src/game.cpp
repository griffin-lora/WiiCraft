#include "game.hpp"
#include "gfx.hpp"

using namespace game;

std::size_t game::get_face_vertex_count(block::type type, block::face face) {
    switch (type) {
        case block::type::AIR: {
            return 0;
        }
        case block::type::GRASS: {
            return 4;
        }
    }
}

void game::add_face_vertices_at_mut_it(math::vector3u8 l /* local_pos */, chunk::mesh::vertex::it& it, block::type type, block::face face) {
    switch (type) {
        case block::type::AIR: {
            return;
        }
        case block::type::GRASS: {
            math::vector2u8 u /* uv_pos */ = { 0, 0 };
            math::vector2u8 uo /* uv_pos_offset */ = { u.x + 1, u.y + 1 };

            math::vector3u8 lo /* local_pos_offset */ = { l.x + 1, l.y + 1, l.z + 1 };

            switch (face) {
                default: {
                    return;
                }
                case block::face::front: {
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
                    return;
                }
                case block::face::back: {
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
                    return;
                }
                case block::face::left: {
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
                    return;
                }
                case block::face::right: {
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
                    return;
                }
                case block::face::top: {
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
                    return;
                }
                case block::face::bottom: {
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
                    return;
                }
            }

            return;
        }
        default: {
            return;
        }
    }
}

void game::draw_chunk_mesh(chunk::mesh::vertex::const_it it, chunk::mesh::vertex::const_it end) {
    gfx::draw_quads(end - it, [&it, end]() {
        for (; it != end; ++it) {
            gfx::draw_vertex((f32)it->local_position.x, (f32)it->local_position.y, (f32)it->local_position.z, ((f32)it->uv_position.x)/16.f, ((f32)it->uv_position.y)/16.f);
        }
    });
}