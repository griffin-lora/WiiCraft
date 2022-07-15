#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    /** Vf stands for a vertex function type
     * vf(u8 x, u8 y, u8 z, u8 u, u8 v)
     */
    template<typename Vf>
    void add_block_vertices(Vf& vf, math::vector3u8 block_pos, const block& block) {
        call_with_block_functionality(block.tp, [&vf, &block_pos, &block]<typename Bf>() {
            call_func_on_each_face<void>([&vf, &block_pos, &block]<block::face face>() {
                if (Bf::template get_face_traits<face>(block.st).visible) {
                    Bf::template add_face_vertices<face>(vf, block_pos, block.st);
                }
            });
            Bf::add_general_vertices(vf, block_pos, block.st);
        });
    }
}