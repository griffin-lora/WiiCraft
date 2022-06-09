#include "game.hpp"

using namespace game;

void game::generate_blocks(chunk& chunk) {
    iterate_over_chunk_blocks_and_positions(chunk.blocks, [](auto& block, auto pos) {
        if (pos.y == 2) {
            block = { .tp = block::type::GRASS };
        } else {
            block = { .tp = block::type::AIR };
        }
    });
}