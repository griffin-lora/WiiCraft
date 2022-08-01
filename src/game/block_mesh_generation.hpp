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

    template<typename T>
    struct block_mesh_layers {
        T::standard standard;
        T::tinted tinted;
        T::tinted_decal tinted_decal;
        T::tinted_double_side_alpha tinted_double_side_alpha;

        template<typename F>
        void for_each(const F& func) const {
            func(standard);
            for_each_except_standard(func);
        }

        template<typename F>
        void for_each_except_standard(const F& func) const {
            func(tinted);
            func(tinted_decal);
            func(tinted_double_side_alpha);
        }
    };

    static constexpr std::size_t safe_buffer_overflow_size = 0x100;

    template<typename T, std::size_t MAX_QUAD_COUNT>
    struct block_quad_array {
        using quad_array = ext::data_array<T>;
        quad_array quads;

        inline block_quad_array() : quads(MAX_QUAD_COUNT + safe_buffer_overflow_size) {}

        inline auto begin() { return quads.begin(); }
        inline auto end() { return quads.end(); }
        inline auto begin() const { return quads.begin(); }
        inline auto end() const { return quads.end(); }

        inline T* data() { return quads.data(); }
        inline const T* data() const { return quads.data(); }
    };

    template<typename T>
    struct block_mesh_layers_block_quad_array_getter {
        using standard = typename T::type<block_quad_array<standard_quad, 0x2eff>>;
        using tinted = typename T::type<block_quad_array<tinted_quad, 0x1000>>;
        using tinted_decal = typename T::type<block_quad_array<tinted_decal_quad, 0x1000>>;
        using tinted_double_side_alpha = typename T::type<block_quad_array<tinted_quad, 0x1000>>;
    };

    template<typename T>
    struct block_mesh_layers_single_type_getter {
        using standard = T;
        using tinted = T;
        using tinted_decal = T;
        using tinted_double_side_alpha = T;
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