#include "game.hpp"

using namespace game;

void game::draw_chunk_mesh(chunk::mesh::vertex::const_it it, chunk::mesh::vertex::const_it end) {
    gfx::draw_quads(end - it, [&it, end]() {
        for (; it != end; ++it) {
            gfx::draw_vertex((f32)it->local_position.x, (f32)it->local_position.y, (f32)it->local_position.z, ((f32)it->uv_position.x)/16.f, ((f32)it->uv_position.y)/16.f);
        }
    });
}