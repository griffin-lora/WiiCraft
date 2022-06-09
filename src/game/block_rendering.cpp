#include "game.hpp"

using namespace game;

void game::draw_chunk_mesh_vertices(const ext::fixed_array<chunk::mesh::vertex>& vertices) {
    gfx::draw_quads(vertices.size(), [&vertices]() {
        for (auto& v : vertices) {
            gfx::draw_vertex((f32)v.local_position.x, (f32)v.local_position.y, (f32)v.local_position.z, ((f32)v.uv_position.x)/16.f, ((f32)v.uv_position.y)/16.f);
        }
    });
}