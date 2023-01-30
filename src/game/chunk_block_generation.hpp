#pragma once
#include "chunk.hpp"
#include "math.hpp"
#include "chunk_mesh_generation.hpp"

namespace game {
    mesh_update_state generate_blocks(chunk& chunk, const math::vector3s32& pos);
}