#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);
    
    gfx::draw_quads(chunk.ms.pos_vertices.size(), [&chunk]() {
        for (std::size_t i = 0; i < chunk.ms.pos_vertices.size(); ++i) {
            auto& p = chunk.ms.pos_vertices[i];
            auto& uv = chunk.ms.uv_vertices[i];
            gfx::draw_vertex(p.x, p.y, p.z, uv.x, uv.y);
        }
    });
}