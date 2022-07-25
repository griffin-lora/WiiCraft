#pragma once
#include "chunk.hpp"
#include "math.hpp"

namespace game {
    void generate_blocks(chunk& chunk, const math::vector3s32& pos);
}