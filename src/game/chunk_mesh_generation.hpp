#pragma once
#include "chunk.hpp"
#include "block_mesh_generation.hpp"

namespace game {
    using chunk_quad_building_arrays = block_mesh_layers<quad_array_container>;

    enum class mesh_update_state {
        CONTINUE,
        BREAK
    };

    mesh_update_state update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
    mesh_update_state update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
}