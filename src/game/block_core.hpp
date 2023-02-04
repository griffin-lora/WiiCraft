#pragma once
#include "block.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include <optional>
#include <vector>

namespace game {
    template<block::face FACE, typename T>
    constexpr T get_face_offset_position(T pos) {
        call_face_func_for<FACE, void>(
            [&]() { pos.x += 1; },
            [&]() { pos.x -= 1; },
            [&]() { pos.y += 1; },
            [&]() { pos.y -= 1; },
            [&]() { pos.z += 1; },
            [&]() { pos.z -= 1; }
        );
        return pos;
    }

    template<block::face FACE>
    struct invert_face;

    template<>
    struct invert_face<block::face::front> {
        static constexpr block::face value = block::face::back;
    };

    template<>
    struct invert_face<block::face::back> {
        static constexpr block::face value = block::face::front;
    };

    template<>
    struct invert_face<block::face::top> {
        static constexpr block::face value = block::face::bottom;
    };

    template<>
    struct invert_face<block::face::bottom> {
        static constexpr block::face value = block::face::top;
    };

    template<>
    struct invert_face<block::face::right> {
        static constexpr block::face value = block::face::left;
    };

    template<>
    struct invert_face<block::face::left> {
        static constexpr block::face value = block::face::right;
    };

    bool does_world_position_select_block(const glm::vec3& world_position, const block& block, const glm::vec3& world_block_position);
}