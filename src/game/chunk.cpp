#include "game.hpp"
#include "PerlinNoise.hpp"

using namespace game;

void game::init(chunk& chunk, const math::vector3s32& pos, math::matrix view) {
    guMtxIdentity(chunk.model);
    guMtxTransApply(chunk.model, chunk.model, pos.x * chunk::SIZE, pos.y * chunk::SIZE, pos.z * chunk::SIZE);
    update_model_view(chunk, view);
}

math::vector3u8 game::get_position_from_index(std::size_t index) {
    uint z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    uint y = index / chunk::SIZE * chunk::SIZE;
    uint x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}

void game::generate_blocks(chunk& chunk, const math::vector3s32& chunk_pos, u32 seed) {
    siv::PerlinNoise noise(seed);
    for (u8 x = 0; x < chunk::SIZE; x++) {
        for (u8 z = 0; z < chunk::SIZE; z++) {
            auto world_x = game::get_world_coord_from_local_position(x, chunk_pos.x);
            auto world_z = game::get_world_coord_from_local_position(z, chunk_pos.z);
            auto height = noise.octave2D(world_x / 16.f, world_z / 16.f, 8);

            s32 y_pos = height * chunk::SIZE;

            for (u8 y = 0; y < chunk::SIZE; y++) {
                auto world_y = game::get_world_coord_from_local_position(y, chunk_pos.y);
                auto index = game::get_index_from_position(math::vector3u8{x, y, z});

                if (world_y < y_pos) {
                    chunk.blocks[index] = { .tp = block::type::GRASS };
                } else {
                    chunk.blocks[index] = { .tp = block::type::AIR };
                }
            }
        }
    }
}