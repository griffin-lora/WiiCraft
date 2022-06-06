#include "game.hpp"
#include "gfx.hpp"

using namespace game;

game::vert_it game::add_face_vertices_at(math::vector3u8 local_pos, vert_it it, block::type type, block::face face) {
    switch (type) {
        case block::type::AIR: {
            return it;
        }
        case block::type::GRASS: {
            math::vector2u8 uv_pos = { 0, 0 };
            math::vector2u8 uv_pos_o = { uv_pos.x + 1, uv_pos.y + 1 };

            math::vector3u8 local_pos_o = { local_pos.x + 1, local_pos.y + 1, local_pos.z + 1 };

            switch (face) {
                default: {
                    return it;
                }
                case block::face::front: {
                    *it++ = {
                        { local_pos.x, local_pos.y, local_pos_o.z },		// Top Right Of The Quad (Front)
			            { uv_pos.x, uv_pos.y }
                    };
                    *it++ = {
                        { local_pos_o.x, local_pos.y, local_pos_o.z },	// Top Left Of The Quad (Front)
			            { uv_pos_o.x, uv_pos.y }
                    };
                    *it++ = {
                        { local_pos_o.x, local_pos.y, local_pos.z },	// Bottom Left Of The Quad (Front)
                        { uv_pos_o.x, uv_pos_o.y }
                    };
                    *it++ = {
                        { local_pos.x, local_pos.y, local_pos.z },	// Bottom Right Of The Quad (Front)
                        { uv_pos.x, uv_pos_o.y }
                    };
                    return it;
                }
            }

            return it;
        }
        default: {
            return it;
        }
    }
}

void game::draw_chunk_mesh(const chunk::mesh& mesh) {
    gfx::draw_quads(mesh.vertices.size(), [mesh]() {
        for (const auto& v : mesh.vertices) {
            gfx::draw_vertex((f32)v.local_position.x, (f32)v.local_position.y, (f32)v.local_position.z, ((f32)v.uv_position.x)/16.f, ((f32)v.uv_position.y)/16.f);
        }
    });
}