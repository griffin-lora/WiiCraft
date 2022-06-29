#include "chunk_math.hpp"

using namespace game;

math::vector3u8 game::get_position_from_index(std::size_t index) {
    u32 z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    u32 y = index / chunk::SIZE * chunk::SIZE;
    u32 x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}