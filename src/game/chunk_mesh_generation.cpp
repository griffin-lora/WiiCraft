#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

math::vector3u8 game::get_position_from_index(std::size_t index) {
    uint z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    uint y = index / chunk::SIZE * chunk::SIZE;
    uint x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}

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