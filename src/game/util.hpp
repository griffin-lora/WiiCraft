#pragma once
#include "math.hpp"

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
        typename ...A
    >
    constexpr R call_face_func_for(F0 front, F1 back, F2 top, F3 bottom, F4 right, F5 left, A&&... args) {
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
        }
    }

    template<typename T, typename F>
    constexpr void iterate_positions_in_sphere(T radius, F func) {
        for (T x = -radius; x <= radius; x++) {
            for (T y = -radius; y <= radius; y++) {
                for (T z = -radius; z <= radius; z++) {
                    const glm::vec<3, T, glm::defaultp> pos = {x, y, z};
                    if (math::length_squared(pos) <= (radius * radius)) {
                        func(pos);
                    }
                }
            }
        }
    }
}