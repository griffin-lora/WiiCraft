#pragma once
#include "block.hpp"
#include "util.hpp"
#include "math/box.hpp"
#include <vector>

namespace game {
    template<block::face face, typename T>
    constexpr T get_face_offset_position(T pos) {
        call_face_func_for<face, void>(
            [&]() { pos.x += 1; },
            [&]() { pos.x -= 1; },
            [&]() { pos.y += 1; },
            [&]() { pos.y -= 1; },
            [&]() { pos.z += 1; },
            [&]() { pos.z -= 1; }
        );
        return pos;
    }

    inline bool is_block_fully_transparent(block::type type);
    inline bool is_block_upper_half_transparent(block::type type);

    bool does_world_position_collide_with_block(const glm::vec3& world_position, block::type type, const glm::vec3& world_block_position);
};