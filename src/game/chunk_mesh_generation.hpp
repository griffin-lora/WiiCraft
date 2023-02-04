#pragma once
#include "chunk.hpp"

namespace game {
    enum class mesh_update_state {
        should_continue,
        should_break
    };

    mesh_update_state update_core_mesh(chunk& chunk);
    mesh_update_state update_shell_mesh(chunk& chunk);
}