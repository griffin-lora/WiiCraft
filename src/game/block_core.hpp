#pragma once
#include "block.hpp"
#include "util.hpp"
#include "math/box.hpp"
#include <optional>
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

    inline bool is_block_bottom_half_transparent(const block& block);
    inline bool is_block_top_half_transparent(const block& block);

    bool does_world_position_collide_with_block(const glm::vec3& world_position, const block& block, const glm::vec3& world_block_position);
    std::optional<math::box> get_box_that_collides_with_world_position(const glm::vec3& world_position, const block& block, const glm::vec3& world_block_position);
};