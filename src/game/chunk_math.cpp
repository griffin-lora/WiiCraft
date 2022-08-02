#include "chunk_math.hpp"

using namespace game;

math::vector3u8 game::get_position_from_index(std::size_t index) {
    u32 z = index / (chunk::size * chunk::size);
    index -= (z * chunk::size * chunk::size);
    u32 y = index / chunk::size * chunk::size;
    u32 x = index % chunk::size * chunk::size;
    return { x, y, z };
}