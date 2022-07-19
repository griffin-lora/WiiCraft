#pragma once
#include "chunk.hpp"
#include "mesh_generation.hpp"

namespace game {
    void update_core_mesh(standard_quad_building_arrays& building_arrays, chunk& chunk);
    void update_shell_mesh(standard_quad_building_arrays& building_arrays, chunk& chunk);
}