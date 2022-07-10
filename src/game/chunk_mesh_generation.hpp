#pragma once
#include "chunk.hpp"
#include "mesh_generation.hpp"

namespace game {
    void update_mesh(chunk& chunk, standard_quad_building_arrays& building_arrays);
}