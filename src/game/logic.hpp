#pragma once
#include "character.hpp"
#include "camera.hpp"
#include "chunk.hpp"
#include "block.hpp"
#include "cursor.hpp"
#include "chunk_math.hpp"
#include <optional>
#include <tuple>

namespace game {
    struct raycast {
        glm::vec3 pos;
        math::vector3s32 ch_pos;
        chunk& ch;
        math::vector3u8 bl_pos;
        block& bl;
    };

    std::optional<raycast> get_raycast(const glm::vec3& origin, const glm::vec3& dir, u8 step_count, chunk::map& chunks);

    void update_from_input(
        f32 cam_movement_speed,
        f32 cam_rotation_speed,
        u16 v_width,
        u16 v_height,
        character& character,
        camera& cam,
        chunk::map& chunks,
        cursor& cursor,
        std::optional<raycast>& raycast
    );
};