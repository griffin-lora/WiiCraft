#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);

    GX_SetArray(GX_VA_POS, chunk.ms.pos_vertices.data(), sizeof(chunk::mesh::pos_vertex));
    GX_SetArray(GX_VA_TEX0, chunk.ms.uv_vertices.data(), sizeof(chunk::mesh::uv_vertex));

    auto vertex_count = chunk.ms.pos_vertices.size();
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
    
    for (std::size_t i = 0; i < vertex_count; i++) {
        GX_Position1x16(i);
		GX_TexCoord1x16(i);
    }

    GX_End();
}