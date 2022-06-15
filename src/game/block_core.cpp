#include "block_core.hpp"

using namespace game;

bool game::is_block_visible(block::type type) {
    switch (type) {
        default:
        case block::type::AIR:
            return false;
        case block::type::DEBUG:
        case block::type::GRASS:
        case block::type::DIRT:
            return true;
    }
}