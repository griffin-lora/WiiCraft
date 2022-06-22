#pragma once
#include "block_core.hpp"
#include "block_functionality.hpp"

using namespace game;

inline bool game::is_block_fully_transparent(block::type type) {
    switch (type) {
        default: return false;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::is_fully_transparent();
        )
    }
}

inline bool game::is_block_upper_half_transparent(block::type type) {
    switch (type) {
        default: return false;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::is_upper_half_transparent();
        )
    }
}

inline f32 game::get_block_height(block::type type) {
    switch (type) {
        default: return 0.0f;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::get_height();
        )
    }
}