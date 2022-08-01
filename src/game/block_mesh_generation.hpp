#pragma once
#include "block.hpp"
#include <array>

namespace game {
    struct standard_quad {
        struct vertex {
            math::vector3u8 pos;
            math::vector2u8 uv;
        };

        vertex vert0;
        vertex vert1;
        vertex vert2;
        vertex vert3;
    };

    struct tinted_quad {
        struct vertex {
            math::vector3u8 pos;
            math::vector3u8 color;
            math::vector2u8 uv;
        };

        vertex vert0;
        vertex vert1;
        vertex vert2;
        vertex vert3;
    };

    struct tinted_decal_quad {
        struct vertex {
            math::vector3u8 pos;
            math::vector3u8 color;
            std::array<math::vector2u8, 2> uvs;
        };

        vertex vert0;
        vertex vert1;
        vertex vert2;
        vertex vert3;
    };

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