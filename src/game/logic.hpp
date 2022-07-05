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
    void update_from_input(
        f32 cam_rotation_speed,
        u16 v_width,
        u16 v_height,
        character& character,
        camera& cam,
        chunk::map& chunks,
        cursor& cursor,
        std::optional<block_raycast>& raycast
    );
}