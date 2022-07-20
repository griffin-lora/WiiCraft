#pragma once
#include "chunk.hpp"
#include "block_mesh_generation.hpp"

namespace game {
    void update_core_mesh(block_quad_building_arrays& building_arrays, chunk& chunk);
    void update_shell_mesh(block_quad_building_arrays& building_arrays, chunk& chunk);
}