#pragma once
#include "block.hpp"
namespace game {
    template<typename Bf, typename M, typename F>
    inline void add_block_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 block_pos) {
        Bf::add_faces_vertices(ms_st, get_face_neighbor_block, st, block_pos);
        Bf::add_general_vertices(ms_st, get_face_neighbor_block, st, block_pos);
    }

    template<typename Bf, typename M, typename F>
    inline void add_block_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 block_pos) {
        Bf::add_faces_vertices(ms_st, get_face_neighbor_block, st, block_pos);
    }
}