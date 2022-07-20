#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    template<typename M>
    void add_block_vertices(M& ms_st, math::vector3u8 block_pos, const block& block) {
        call_with_block_functionality(block.tp, [&ms_st, &block_pos, &block]<typename Bf>() {
            call_func_on_each_face<void>([&ms_st, &block_pos, &block]<block::face face>() {
                if (Bf::template get_face_traits<face>(block.st).visible) {
                    Bf::template add_face_vertices<face>(ms_st, block_pos, block.st);
                }
            });
            Bf::add_general_vertices(ms_st, block_pos, block.st);
        });
    }
}