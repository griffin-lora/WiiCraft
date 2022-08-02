#pragma once
#include "block.hpp"
#include "gfx/display_list.hpp"
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

        using chunk_quad = standard_quad;
        static constexpr std::size_t max_quad_count = 0x2eff;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static inline std::size_t get_chunk_display_list_size(std::size_t vert_count) {
            return
                gfx::get_begin_instruction_size(vert_count) +
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
                gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
        }

        static inline void write_chunk_vertex(const chunk_quad::vertex& vert) {
            GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
            GX_TexCoord2u8(vert.uv.x, vert.uv.y);
        }
    };

    struct tinted_block_mesh_layer {
        static constexpr const char* name = "tinted block mesh layer";

        using chunk_quad = tinted_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static inline std::size_t get_chunk_display_list_size(std::size_t vert_count) {
            return
                gfx::get_begin_instruction_size(vert_count) +
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
                gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
        }

        static inline void write_chunk_vertex(const chunk_quad::vertex& vert) {
            GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
            GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
            GX_TexCoord2u8(vert.uv.x, vert.uv.y);
        }
    };

    struct tinted_decal_block_mesh_layer {
        static constexpr const char* name = "tinted decal block mesh layer";

        using chunk_quad = tinted_decal_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static inline std::size_t get_chunk_display_list_size(std::size_t vert_count) {
            return
                gfx::get_begin_instruction_size(vert_count) +
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
                gfx::get_vector_instruction_size<2, u8>(vert_count) + // UV
                gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
        }

        static inline void write_chunk_vertex(const chunk_quad::vertex& vert) {
            GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
            GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
            GX_TexCoord2u8(vert.uvs[0].x, vert.uvs[0].y);
            GX_TexCoord2u8(vert.uvs[1].x, vert.uvs[1].y);
        }
    };

    struct tinted_double_side_alpha_block_mesh_layer {
        static constexpr const char* name = "tinted double side alpha block mesh layer";

        using chunk_quad = tinted_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static inline std::size_t get_chunk_display_list_size(std::size_t vert_count) {
            return
                gfx::get_begin_instruction_size(vert_count) +
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Position
                gfx::get_vector_instruction_size<3, u8>(vert_count) + // Color
                gfx::get_vector_instruction_size<2, u8>(vert_count); // UV
        }

        static inline void write_chunk_vertex(const chunk_quad::vertex& vert) {
            GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
            GX_Color3u8(vert.color.r, vert.color.g, vert.color.b);
            GX_TexCoord2u8(vert.uv.x, vert.uv.y);
        }
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

        template<typename L>
        const auto& get_layer() const {
            if constexpr (std::is_same_v<L, standard_block_mesh_layer>) {
                return standard;
            } else if constexpr (std::is_same_v<L, tinted_block_mesh_layer>) {
                return tinted;
            } else if constexpr (std::is_same_v<L, tinted_decal_block_mesh_layer>) {
                return tinted_decal;
            } else if constexpr (std::is_same_v<L, tinted_double_side_alpha_block_mesh_layer>) {
                return tinted_double_side_alpha;
            }
        }

        template<typename L>
        auto& get_layer() {
            if constexpr (std::is_same_v<L, standard_block_mesh_layer>) {
                return standard;
            } else if constexpr (std::is_same_v<L, tinted_block_mesh_layer>) {
                return tinted;
            } else if constexpr (std::is_same_v<L, tinted_decal_block_mesh_layer>) {
                return tinted_decal;
            } else if constexpr (std::is_same_v<L, tinted_double_side_alpha_block_mesh_layer>) {
                return tinted_double_side_alpha;
            }
        }
    };
    
    template<typename T>
    struct single_type_container {
        template<typename T1>
        using type = T;
    };

    struct chunk_quad_array_container {
        template<typename T>
        using type = T::chunk_quad_array;
    };

    struct chunk_quad_array_iterator_container {
        template<typename T>
        using type = T::chunk_quad_array::iterator;
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