#pragma once
#include "chunk.hpp"
#include "math.hpp"

namespace game {
    math::vector3u8 get_position_from_index(std::size_t index);
    template<typename T>
    inline std::size_t get_index_from_position(T position) {
        return position.x + (position.y * chunk::SIZE) + (position.z * chunk::SIZE * chunk::SIZE);
    }
    inline s32 get_world_coord_from_local_position(s32 local_coord, s32 chunk_coord) {
        return ((chunk_coord * chunk::SIZE) + local_coord);
    }
    template<typename O, typename T>
    inline O floor_float_position(const T& position) {
        return {
            std::floor(position.x),
            std::floor(position.y),
            std::floor(position.z)
        };
    }
    template<typename T>
    inline math::vector3s32 get_chunk_position_from_world_position(const T& world_position) {
        return floor_float_position<math::vector3s32>(world_position / (f32)chunk::SIZE);
    }

    template<typename T>
    inline math::vector3u8 get_local_block_position(const T& world_position) {
        return {
            math::mod((u8)world_position.x, (u8)chunk::SIZE),
            math::mod((u8)world_position.y, (u8)chunk::SIZE),
            math::mod((u8)world_position.z, (u8)chunk::SIZE)
        };
    }

    // Weird hack
    template<typename T>
    inline math::vector3s32 get_local_block_position_in_s32(const T& world_position) {
        return {
            math::mod((s32)world_position.x, chunk::SIZE),
            math::mod((s32)world_position.y, chunk::SIZE),
            math::mod((s32)world_position.z, chunk::SIZE)
        };
    }

    inline math::vector3u8 get_local_block_position(const glm::vec3& world_position) {
        return {
            math::mod((u8)std::floor(world_position.x), (u8)chunk::SIZE),
            math::mod((u8)std::floor(world_position.y), (u8)chunk::SIZE),
            math::mod((u8)std::floor(world_position.z), (u8)chunk::SIZE)
        };
    }
}