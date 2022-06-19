#include "block_core.hpp"
#include "block_type.hpp"

using namespace game;

#define EVAL_IS_BLOCK_VISIBLE_CASE(tp) case block::type::tp: return block_type<block::type::tp>::is_visible();

bool game::is_block_visible(block::type type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_VISIBLE_CASE)
    }
}