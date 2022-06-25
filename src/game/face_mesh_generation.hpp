#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    /** Vf stands for a vertex function type
     * vf(u8 x, u8 y, u8 z, u8 u, u8 v)
     */
    std::size_t get_block_vertex_count(const block& block);
    template<typename Vf>
    void add_block_vertices(Vf& vf, math::vector3u8 block_pos, const block& block) {
        switch (block.tp) {
            default: break;
            EVAL_BLOCK_FUNCTIONALITY_CASES(
                Bf::add_face_vertices<block::face::FRONT>(vf, block_pos, block.st);
                Bf::add_face_vertices<block::face::BACK>(vf, block_pos, block.st);
                Bf::add_face_vertices<block::face::TOP>(vf, block_pos, block.st);
                Bf::add_face_vertices<block::face::BOTTOM>(vf, block_pos, block.st);
                Bf::add_face_vertices<block::face::RIGHT>(vf, block_pos, block.st);
                Bf::add_face_vertices<block::face::LEFT>(vf, block_pos, block.st);
                Bf::add_general_vertices(vf, block_pos, block.st);
                break;
            )
        }
    }
}