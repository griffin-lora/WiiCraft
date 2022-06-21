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

    inline bool is_block_visible(block::type type);
    inline bool is_block_semitransparent(block::type type);

    template<block::face face>
    inline bool is_block_face_visible(block::type type, block::type check_type);
};