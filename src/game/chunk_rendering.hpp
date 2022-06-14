#pragma once
#include "chunk.hpp"

namespace game {
    void draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices);
    void draw_chunk(chunk& chunk);
};