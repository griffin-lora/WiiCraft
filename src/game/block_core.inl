#pragma once
#include "block_core.hpp"
#include "block_functionality.hpp"

inline game::block_traits game::get_block_traits(const block& block) {
    return get_with_block_functionality<block_traits>(block.tp, [&block]<typename Bf>() {
        return Bf::get_block_traits(block.st);
    });
}

template<game::block::face face>
inline game::face_traits game::get_neighbor_face_traits(const block& block) {
    return get_with_block_functionality<face_traits>(block.tp, [&block]<typename Bf>() {
        return Bf::template get_face_traits<invert_face<face>::value>(block.st);
    });
}