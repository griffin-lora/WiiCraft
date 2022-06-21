#pragma once
#include "block_core.hpp"
#include "block_functionality.hpp"

using namespace game;


#define EVAL_IS_BLOCK_VISIBLE_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_visible();

inline bool game::is_block_visible(block::type type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_VISIBLE_CASE)
    }
}

#define EVAL_IS_BLOCK_FULLY_TRANSPARENT_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_fully_transparent();

inline bool game::is_block_fully_transparent(block::type type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_FULLY_TRANSPARENT_CASE)
    }
}

#define EVAL_IS_BLOCK_UPPER_HALF_TRANSPARENT_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_upper_half_transparent();

inline bool game::is_block_upper_half_transparent(block::type type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_UPPER_HALF_TRANSPARENT_CASE)
    }
}

#define EVAL_IS_BLOCK_FACE_VISIBLE_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_face_visible<face>(neighbor_type);

template<block::face face>
bool game::is_block_face_visible(block::type type, block::type neighbor_type) {
    switch (type) {
        default: return false;
        EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_FACE_VISIBLE_CASE)
    }
}