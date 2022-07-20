#pragma once
#include "chunk.hpp"
#include "math.hpp"

namespace game {
    math::vector3u8 get_position_from_index(std::size_t index);
    template<typename O, typename T>
    inline O get_index_from_position(T position) {
        return position.x + (position.y * chunk::SIZE) + (position.z * chunk::SIZE * chunk::SIZE);
    }
    template<typename O, typename T>
    inline glm::vec<3, O, glm::defaultp> floor_float_position(const T& position) {
        return {
            (O)std::floor(position.x),
            (O)std::floor(position.y),
            (O)std::floor(position.z)
        };
    }
    template<typename T>
    inline math::vector3s32 get_chunk_position_from_world_position(const glm::vec<3, T, glm::defaultp>& world_position) {
        return floor_float_position<s32>(world_position / (T)chunk::SIZE);
    }

    template<typename O, typename T>
    inline glm::vec<3, O, glm::defaultp> get_local_block_position(const T& world_position) {
        return {
            math::mod((O)world_position.x, (O)chunk::SIZE),
            math::mod((O)world_position.y, (O)chunk::SIZE),
            math::mod((O)world_position.z, (O)chunk::SIZE)
        };
    }
    
    template<typename O>
    inline glm::vec<3, O, glm::defaultp> get_local_block_position(const glm::vec3& world_position) {
        return {
            math::mod((O)std::floor(world_position.x), (O)chunk::SIZE),
            math::mod((O)std::floor(world_position.y), (O)chunk::SIZE),
            math::mod((O)std::floor(world_position.z), (O)chunk::SIZE)
        };
    }
}