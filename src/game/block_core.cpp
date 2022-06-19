#include "block_core.hpp"
#include "block_functionality.hpp"

using namespace game;

#define EVAL_IS_BLOCK_SOLID_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_solid();

bool game::is_block_solid(block::type type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_SOLID_CASE)
    }
}