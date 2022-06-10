#include "game.hpp"

using namespace game;

void game::init(chunk& chunk, math::matrix view) {
    guMtxIdentity(chunk.model);
    guMtxTransApply(chunk.model, chunk.model, chunk.position.x * chunk::SIZE, chunk.position.y * chunk::SIZE, chunk.position.z * chunk::SIZE);
    update_model_view(chunk, view);
}

math::vector3u8 game::get_position_from_index(std::size_t index) {
    uint z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    uint y = index / chunk::SIZE * chunk::SIZE;
    uint x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}

void game::generate_blocks(chunk& chunk) {
    iterate_over_chunk_blocks_and_positions(chunk.blocks, [](auto& block, auto pos) {
        if (pos.y <= 2) {
            block = { .tp = block::type::GRASS };
        } else {
            block = { .tp = block::type::AIR };
        }
    });
}