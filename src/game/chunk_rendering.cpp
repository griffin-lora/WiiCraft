#include "game.hpp"

using namespace game;

void game::init(chunk& chunk, math::matrix view) {
    guMtxIdentity(chunk.model);
    guMtxTransApply(chunk.model, chunk.model, chunk.position.x * chunk::SIZE, chunk.position.y * chunk::SIZE, chunk.position.z * chunk::SIZE);
    update_model_view(chunk, view);
}

void game::draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices) {
    gfx::draw_quads(vertices.size(), [&vertices]() {
        for (auto& v : vertices) {
            gfx::draw_vertex((f32)v.local_position.x, (f32)v.local_position.y, (f32)v.local_position.z, ((f32)v.uv_position.x)/16.f, ((f32)v.uv_position.y)/16.f);
        }
    });
}

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);

    game::draw_chunk_mesh_vertices(chunk.ms.vertices);
}