#pragma once
#include "math.hpp"
#include <functional>

using error_code = int;

namespace std {
    template<>
    struct hash<math::vector3s32> {
        std::size_t operator()(const math::vector3s32& vec) const {
            return (std::hash<s32>()(vec.x) ^ (std::hash<s32>()(vec.y) << 1) >> 1) ^ (std::hash<s32>()(vec.z) << 1);
        }
    };
}