#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

void game::draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices) {
    gfx::draw_quads(vertices.size(), [&vertices]() {
        for (auto& v : vertices) {
            gfx::draw_vertex(v.local_position.x, v.local_position.y, v.local_position.z, v.uv_position.x, v.uv_position.y);
        }
    });
}

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);

    game::draw_chunk_mesh_vertices(chunk.ms.vertices);
}