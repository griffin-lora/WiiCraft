#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    template<block::face face>
    std::size_t get_face_vertex_count(block::type type) {
        switch (type) {
            default: return 0;
            EVAL_BLOCK_FUNCTIONALITY_CASES(
                return Bf::get_face_vertex_count<face>();
            )
        }
    }

    inline std::size_t get_general_vertex_count(block::type type) {
        switch (type) {
            default: return 0;
            EVAL_BLOCK_FUNCTIONALITY_CASES(
                return Bf::get_general_vertex_count();
            )
        }
    }

    /** Vf stands for a vertex function type
     * vf(u8 x, u8 y, u8 z, u8 u, u8 v)
     */

    template<block::face face, typename Vf>
    void add_face_vertices(Vf& vf, math::vector3u8 local_position, block::type type) {
        switch (type) {
            default: break;
            EVAL_BLOCK_FUNCTIONALITY_CASES(X(
                Bf::add_face_vertices<face, Vf>(vf, local_position); break;
            ))
        }
    }

    template<typename Vf>
    void add_general_vertices(Vf& vf, math::vector3u8 local_position, block::type type) {
        switch (type) {
            default: break;
            EVAL_BLOCK_FUNCTIONALITY_CASES(X(
                Bf::add_general_vertices<Vf>(vf, local_position); break;
            ))
        }
    }

    std::size_t get_block_vertex_count(block::type type);
    template<typename Vf>
    void add_block_vertices(Vf& vf, math::vector3u8 local_position, block::type type) {
        add_face_vertices<block::face::FRONT, Vf>(vf, local_position, type);
        add_face_vertices<block::face::BACK, Vf>(vf, local_position, type);
        add_face_vertices<block::face::TOP, Vf>(vf, local_position, type);
        add_face_vertices<block::face::BOTTOM, Vf>(vf, local_position, type);
        add_face_vertices<block::face::RIGHT, Vf>(vf, local_position, type);
        add_face_vertices<block::face::LEFT, Vf>(vf, local_position, type);
        add_general_vertices<Vf>(vf, local_position, type);
    }
};