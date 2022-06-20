#pragma once
#include "block_functionality.hpp"

// Had to put this in a file because circular dependency :)

namespace game {
    #define EVAL_IS_BLOCK_VISIBLE_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::is_face_visible<face>(check_type);

    template<block::face face>
    bool is_block_face_visible(block::type type, block::type check_type) {
        switch (type) {
            default: return false;
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_IS_BLOCK_VISIBLE_CASE)
        }
    }
};