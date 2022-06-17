#include "chunk_rendering.hpp"
#include "gfx.hpp"

using namespace game;

void game::draw_chunk(chunk& chunk) {
    // load the modelview matrix into matrix memory
    gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
    
    gfx::set_position_matrix_from_index(GX_PNMTX3);

    chunk.disp_list.call();
}