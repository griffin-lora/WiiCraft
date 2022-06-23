#pragma once
#include "block_core.hpp"
#include "block_functionality.hpp"

using namespace game;

inline bool game::is_block_bottom_half_transparent(const block& block) {
    switch (block.tp) {
        default: return false;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::is_bottom_half_transparent(block.st);
        )
    }
}

inline bool game::is_block_top_half_transparent(const block& block) {
    switch (block.tp) {
        default: return false;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::is_top_half_transparent(block.st);
        )
    }
}