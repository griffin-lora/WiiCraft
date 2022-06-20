#pragma once
#include "chunk_core.hpp"
#include "block_functionality.hpp"

namespace game {
    #define EVAL_GET_FACE_VERTEX_COUNT_CASE(tp) case block::type::tp: return block_functionality<block::type::tp>::get_face_vertex_count<face>();

    template<block::face face>
    std::size_t get_face_vertex_count(block::type type) {
        switch (type) {
            default: return 0;
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_GET_FACE_VERTEX_COUNT_CASE)
        }
    }

    /** Vf stands for a vertex function type
     * vf(u8 x, u8 y, u8 z, s8 nx, s8 ny, s8 nz, u8 u, u8 v)
     */

    #define EVAL_ADD_FACE_VERTICES_CASE(tp) case block::type::tp: block_functionality<block::type::tp>::add_face_vertices<face, Vf>(vf, local_position); break;

    template<block::face face, typename Vf>
    void add_face_vertices(Vf& vf, math::vector3u8 local_position, block::type type) {
        switch (type) {
            default: break;
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_ADD_FACE_VERTICES_CASE)
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
        // TODO: make this work
        // add_face_vertices<block::face::CENTER, Vf>(local_position, type);
    }
};