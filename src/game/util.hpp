#pragma once
#include "math.hpp"

namespace game {
    template<typename T, typename F>
    constexpr void iterate_positions_in_sphere(T radius, F func) {
        for (T x = -radius; x <= radius; x++) {
            for (T y = -radius; y <= radius; y++) {
                for (T z = -radius; z <= radius; z++) {
                    glm::vec<3, T, glm::defaultp> pos = {x, y, z};
                    if (math::squared_length(pos) <= (radius * radius)) {
                        func(pos);
                    }
                }
            }
        }
    }
}