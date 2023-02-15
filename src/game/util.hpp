#pragma once
#include "game_math.hpp"

namespace game {
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