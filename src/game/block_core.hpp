#pragma once
#include "block.hpp"
#include "math/box.hpp"
#include "traits.hpp"
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

    template<block::face face>
    struct invert_face;

    template<>
    struct invert_face<block::face::FRONT> {
        static constexpr block::face value = block::face::BACK;
    };

    template<>
    struct invert_face<block::face::BACK> {
        static constexpr block::face value = block::face::FRONT;
    };

    template<>
    struct invert_face<block::face::TOP> {
        static constexpr block::face value = block::face::BOTTOM;
    };

    template<>
    struct invert_face<block::face::BOTTOM> {
        static constexpr block::face value = block::face::TOP;
    };

    template<>
    struct invert_face<block::face::RIGHT> {
        static constexpr block::face value = block::face::LEFT;
    };

    template<>
    struct invert_face<block::face::LEFT> {
        static constexpr block::face value = block::face::RIGHT;
    };

    inline block_traits get_block_traits(const block& block);

    template<block::face face>
    inline face_traits get_neighbor_face_traits(const block& block);

    bool does_world_position_select_block(const glm::vec3& world_position, const block& block, const glm::vec3& world_block_position);
    std::vector<math::box> get_block_boxes_that_collide_with_world_box(const math::box& world_box, const block& block, const glm::vec3& world_block_position);
}