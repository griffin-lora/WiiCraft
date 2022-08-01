#pragma once
#include "chunk.hpp"
#include "block_mesh_generation.hpp"

namespace game {
    struct chunk_quad_building_arrays {
        ext::data_array<standard_quad> standard;
        ext::data_array<tinted_quad> tinted;
        ext::data_array<tinted_decal_quad> tinted_decal;
        ext::data_array<tinted_quad> tinted_double_side_alpha;

        chunk_quad_building_arrays();
    };

    enum class mesh_update_state {
        CONTINUE,
        BREAK
    };

    mesh_update_state update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
    mesh_update_state update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
}