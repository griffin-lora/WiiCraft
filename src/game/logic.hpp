#pragma once
#include "block_raycast.hpp"
#include "character.hpp"
#include "camera.hpp"
#include "block.hpp"
#include "cursor.hpp"
#include "game_math.hpp"
#include <optional>
#include <tuple>

namespace game {
    void update_world_from_raycast_and_input(s32vec3s corner_pos, u32 buttons_down, block_raycast_wrap_t& raycast);
}