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

    static constexpr std::size_t safe_buffer_overflow_size = 0x100;

    template<typename T, std::size_t MAX_QUAD_COUNT>
    struct block_quad_array {
        using iterator = ext::data_array<T>::iterator;
        ext::data_array<T> quads;

        inline block_quad_array() : quads(MAX_QUAD_COUNT + safe_buffer_overflow_size) {}

        inline auto begin() { return quads.begin(); }
        inline auto end() { return quads.end(); }
        inline auto begin() const { return quads.begin(); }
        inline auto end() const { return quads.end(); }

        inline T* data() { return quads.data(); }
        inline const T* data() const { return quads.data(); }
    };

    struct standard_block_mesh_layer {
        static constexpr const char* name = "standard layer";

        static constexpr std::size_t max_quad_count = 0x2eff;
        using quad_array = block_quad_array<standard_quad, max_quad_count>;
    };

    struct tinted_block_mesh_layer {
        static constexpr const char* name = "tinted block mesh layer";

        static constexpr std::size_t max_quad_count = 0x1000;
        using quad_array = block_quad_array<tinted_quad, max_quad_count>;
    };

    struct tinted_decal_block_mesh_layer {
        static constexpr const char* name = "tinted decal block mesh layer";

        static constexpr std::size_t max_quad_count = 0x1000;
        using quad_array = block_quad_array<tinted_decal_quad, max_quad_count>;
    };

    struct tinted_double_side_alpha_block_mesh_layer {
        static constexpr const char* name = "tintel double side alpha block mesh layer";

        static constexpr std::size_t max_quad_count = 0x1000;
        using quad_array = block_quad_array<tinted_quad, max_quad_count>;
    };

    template<typename F>
    void for_each_block_mesh_layer(const F& func) {
        func.template operator()<standard_block_mesh_layer>();
        func.template operator()<tinted_block_mesh_layer>();
        func.template operator()<tinted_decal_block_mesh_layer>();
        func.template operator()<tinted_double_side_alpha_block_mesh_layer>();
    }

    template<typename C>
    struct block_mesh_layers {
        C::template type<standard_block_mesh_layer> standard;
        C::template type<tinted_block_mesh_layer> tinted;
        C::template type<tinted_decal_block_mesh_layer> tinted_decal;
        C::template type<tinted_double_side_alpha_block_mesh_layer> tinted_double_side_alpha;

        block_mesh_layers() = default;

        template<typename F>
        block_mesh_layers(const F& func) :
            standard(func.template operator()<standard_block_mesh_layer>()),
            tinted(func.template operator()<tinted_block_mesh_layer>()),
            tinted_decal(func.template operator()<tinted_decal_block_mesh_layer>()),
            tinted_double_side_alpha(func.template operator()<tinted_double_side_alpha_block_mesh_layer>()) {}

        template<typename T>
        const auto& get_layer() const {
            if constexpr (std::is_same_v<T, standard_block_mesh_layer>) {
                return standard;
            } else if constexpr (std::is_same_v<T, tinted_block_mesh_layer>) {
                return tinted;
            } else if constexpr (std::is_same_v<T, tinted_decal_block_mesh_layer>) {
                return tinted_decal;
            } else if constexpr (std::is_same_v<T, tinted_double_side_alpha_block_mesh_layer>) {
                return tinted_double_side_alpha;
            }
        }

        template<typename T>
        auto& get_layer() {
            if constexpr (std::is_same_v<T, standard_block_mesh_layer>) {
                return standard;
            } else if constexpr (std::is_same_v<T, tinted_block_mesh_layer>) {
                return tinted;
            } else if constexpr (std::is_same_v<T, tinted_decal_block_mesh_layer>) {
                return tinted_decal;
            } else if constexpr (std::is_same_v<T, tinted_double_side_alpha_block_mesh_layer>) {
                return tinted_double_side_alpha;
            }
        }
    };
    
    template<typename T>
    struct single_type_container {
        template<typename T1>
        using type = T;
    };

    struct quad_array_container {
        template<typename T>
        using type = T::quad_array;
    };

    struct quad_array_iterator_container {
        template<typename T>
        using type = T::quad_array::iterator;
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