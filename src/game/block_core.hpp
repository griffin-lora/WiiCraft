#pragma once
#include "block.hpp"
#include "math/box.hpp"
#include <optional>
#include <vector>

namespace game {
    template<block_face_t FACE, typename T>
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
}