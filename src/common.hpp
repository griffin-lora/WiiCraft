#pragma once
#include "math.hpp"
#include <functional>

static constexpr s32 CHUNK_SIZE = 32;

struct hash_vector3s32 {
    inline std::size_t operator()(const math::vector3s32& vec) const {
        return (std::hash<s32>()(vec.x) ^ (std::hash<s32>()(vec.y) << 1) >> 1) ^ (std::hash<s32>()(vec.z) << 1);
    }
};