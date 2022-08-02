#pragma once
#include "block_mesh_generation.hpp"

namespace game {

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

        static std::size_t get_chunk_display_list_size(std::size_t vert_count);
        static void write_chunk_vertex(const chunk_quad::vertex& vert);
    };

    struct tinted_block_mesh_layer {
        static constexpr const char* name = "tinted block mesh layer";

        using chunk_quad = tinted_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static std::size_t get_chunk_display_list_size(std::size_t vert_count);
        static void write_chunk_vertex(const chunk_quad::vertex& vert);
    };

    struct tinted_decal_block_mesh_layer {
        static constexpr const char* name = "tinted decal block mesh layer";

        using chunk_quad = tinted_decal_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static std::size_t get_chunk_display_list_size(std::size_t vert_count);
        static void write_chunk_vertex(const chunk_quad::vertex& vert);
    };

    struct tinted_double_side_alpha_block_mesh_layer {
        static constexpr const char* name = "tinted double side alpha block mesh layer";

        using chunk_quad = tinted_quad;
        static constexpr std::size_t max_quad_count = 0x1000;
        using chunk_quad_array = block_quad_array<chunk_quad, max_quad_count>;

        static std::size_t get_chunk_display_list_size(std::size_t vert_count);
        static void write_chunk_vertex(const chunk_quad::vertex& vert);
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
};