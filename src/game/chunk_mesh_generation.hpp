#pragma once
#include "chunk.hpp"
#include "mesh_generation.hpp"

namespace game {
    void update_mesh(const block::neighborhood_lookups& lookups, standard_quad_building_arrays& building_arrays, chunk& chunk);
}