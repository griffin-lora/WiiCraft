#pragma once
#include "block.hpp"

namespace game {
    template<block::type type>
    struct block_type {
        static inline bool is_visible() { return false; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 0; }
    };

    template<>
    struct block_type<block::type::DEBUG> {
        static inline bool is_visible() { return true; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 4; }
    };

    template<>
    struct block_type<block::type::GRASS> {
        static inline bool is_visible() { return true; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 4; }
    };

    template<>
    struct block_type<block::type::STONE> {
        static inline bool is_visible() { return true; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 4; }
    };

    template<>
    struct block_type<block::type::DIRT> {
        static inline bool is_visible() { return true; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 4; }
    };

    template<>
    struct block_type<block::type::WOOD_PLANKS> {
        static inline bool is_visible() { return true; }

        template<block::face face>
        static inline std::size_t get_face_vertex_count() { return 4; }
    };
};