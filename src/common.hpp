#pragma once
#include "math.hpp"
#include <functional>

struct hash_vector3s32 {
    inline std::size_t operator()(const math::vector3s32& vec) const {
        return (std::hash<s32>()(vec.x) ^ (std::hash<s32>()(vec.y) << 1) >> 1) ^ (std::hash<s32>()(vec.z) << 1);
    }
};