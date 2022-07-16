#pragma once
#include "block_raycast.hpp"
#include "character.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "block.hpp"
#include "cursor.hpp"
#include "chunk_math.hpp"
#include <optional>
#include <tuple>

namespace game {
    void update_world_from_raycast_and_input(chunk::map& chunks, u32 buttons_down, std::optional<block_raycast>& raycast);
}