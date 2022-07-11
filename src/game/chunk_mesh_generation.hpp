#pragma once
#include "chunk.hpp"
#include "mesh_generation.hpp"

namespace game {
    void update_mesh(const ext::data_array<chunk::block_position_index_pair>& position_index_pairs, standard_quad_building_arrays& building_arrays, chunk& chunk);
}