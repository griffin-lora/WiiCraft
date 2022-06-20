#pragma once
#include "block.hpp"

namespace game {
    template<
        block::face face,
        typename R,
        typename F0,
        typename F1,
        typename F2,
        typename F3,
        typename F4,
        typename F5,
        typename F6,
        typename ...A
    >
    constexpr R call_face_func_for(F0 front, F1 back, F2 top, F3 bottom, F4 right, F5 left, F6 center, A&&... args) {
        if constexpr (face == block::face::FRONT) {
            return front(std::forward<A>(args)...);
        } else if constexpr (face == block::face::BACK) {
            return back(std::forward<A>(args)...);
        } else if constexpr (face == block::face::TOP) {
            return top(std::forward<A>(args)...);
        } else if constexpr (face == block::face::BOTTOM) {
            return bottom(std::forward<A>(args)...);
        } else if constexpr (face == block::face::RIGHT) {
            return right(std::forward<A>(args)...);
        } else if constexpr (face == block::face::LEFT) {
            return left(std::forward<A>(args)...);
        } else if constexpr (face == block::face::CENTER) {
            return center(std::forward<A>(args)...);
        }
    }

    template<block::face face, typename T>
    constexpr T get_face_offset_position(T pos) {
        static_assert(face != block::face::CENTER, "Center face is not allowed.");
        call_face_func_for<face, void>(
            [&]() { pos.x += 1; },
            [&]() { pos.x -= 1; },
            [&]() { pos.y += 1; },
            [&]() { pos.y -= 1; },
            [&]() { pos.z += 1; },
            [&]() { pos.z -= 1; },
            []() {}
        );
        return pos;
    }

    bool is_block_visible(block::type type);
    bool is_block_solid(block::type type);
};