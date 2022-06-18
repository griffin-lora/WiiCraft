#pragma once
#include "camera.hpp"
#include "chunk.hpp"
#include "block.hpp"
#include "cursor.hpp"
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

    std::optional<raycast> get_raycast(const camera& cam, chunk::map& chunks);

    void update_from_input(
        f32 cam_movement_speed,
        f32 cam_rotation_speed,
        u16 v_width,
        u16 v_height,
        camera& cam,
        chunk::map& chunks,
        cursor& cursor,
        std::optional<raycast>& raycast
    );
};