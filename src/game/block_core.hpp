#pragma once
#include "block.hpp"
#include "util.hpp"

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

    bool is_block_visible(block::type type);
    bool is_block_solid(block::type type);
};