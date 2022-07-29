#pragma once
#include "chunk.hpp"
#include "chunk_mesh_generation.hpp"

namespace game {
    struct chunk_quad_building_arrays {
        static constexpr std::size_t SAFE_BUFFER_OVERFLOW_SIZE = 0x100;

        using quad_array = ext::data_array<chunk::quad>;
        quad_array standard;
        quad_array grass;
        quad_array foliage;
        quad_array transparent;

        inline chunk_quad_building_arrays() : standard(chunk::MAX_STANDARD_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE), grass(chunk::MAX_GRASS_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE), foliage(chunk::MAX_FOLIAGE_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE), transparent(chunk::MAX_TRANSPARENT_QUAD_COUNT + SAFE_BUFFER_OVERFLOW_SIZE) {}
    };

    enum class mesh_update_state {
        CONTINUE,
        BREAK
    };

    mesh_update_state update_core_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
    mesh_update_state update_shell_mesh(chunk_quad_building_arrays& building_arrays, chunk& chunk);
}