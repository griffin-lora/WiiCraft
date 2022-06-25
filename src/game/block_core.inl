#pragma once
#include "block_core.hpp"
#include "block_functionality.hpp"

using namespace game;

inline block_traits game::get_block_traits(const block& block) {
    switch (block.tp) {
        default: return {};
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::get_block_traits(block.st);
        )
    }
}

template<block::face face>
inline face_traits game::get_neighbor_face_traits(const block& block) {
    switch (block.tp) {
        default: return {};
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return Bf::get_face_traits<invert_face<face>::value>(block.st);
        )
    }
}