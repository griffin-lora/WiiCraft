#pragma once
#include "chunk.hpp"
#include "block_mesh_layers.hpp"

namespace game {
    using chunk_quad_building_arrays = block_mesh_layers<chunk_quad_array_container>;

    enum class mesh_update_state {
        should_continue,
        should_break
    };

    mesh_update_state update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
    mesh_update_state update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
}