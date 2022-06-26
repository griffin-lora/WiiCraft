#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    /** Vf stands for a vertex function type
     * vf(u8 x, u8 y, u8 z, u8 u, u8 v)
     */
    template<block::face face, typename Bf, typename Vf>
    void add_visible_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
        if (Bf::template get_face_traits<face>(st).visible) {
            Bf::template add_face_vertices<face>(vf, block_pos, st);
        }
    }

    template<typename Vf>
    void add_block_vertices(Vf& vf, math::vector3u8 block_pos, const block& block) {
        switch (block.tp) {
            default: break;
            EVAL_BLOCK_FUNCTIONALITY_CASES(X(
                add_visible_face_vertices<block::face::FRONT, Bf>(vf, block_pos, block.st);
                add_visible_face_vertices<block::face::BACK, Bf>(vf, block_pos, block.st);
                add_visible_face_vertices<block::face::TOP, Bf>(vf, block_pos, block.st);
                add_visible_face_vertices<block::face::BOTTOM, Bf>(vf, block_pos, block.st);
                add_visible_face_vertices<block::face::RIGHT, Bf>(vf, block_pos, block.st);
                add_visible_face_vertices<block::face::LEFT, Bf>(vf, block_pos, block.st);
                Bf::add_general_vertices(vf, block_pos, block.st);
                break;
            ))
        }
    }
}