#pragma once
#include "chunk.hpp"
#include "block_mesh_generation.hpp"

namespace game {
    struct identity_getter {
        template<typename T>
        using type = T;
    };

    using chunk_quad_building_arrays = block_mesh_layers<block_mesh_layers_block_quad_array_getter<identity_getter>>;

    enum class mesh_update_state {
        CONTINUE,
        BREAK
    };

    mesh_update_state update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
    mesh_update_state update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
}