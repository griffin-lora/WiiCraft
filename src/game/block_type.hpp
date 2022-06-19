#pragma once
#include "block.hpp"

namespace game {
    template<block::type type>
    struct block_type {
        static constexpr bool is_visible() { return false; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { return 0; }
    };

    template<>
    struct block_type<block::type::DEBUG> {
        static constexpr bool is_visible() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; }
    };

    template<>
    struct block_type<block::type::GRASS> {
        static constexpr bool is_visible() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; }
    };

    template<>
    struct block_type<block::type::STONE> {
        static constexpr bool is_visible() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; }
    };

    template<>
    struct block_type<block::type::DIRT> {
        static constexpr bool is_visible() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; }
    };

    template<>
    struct block_type<block::type::WOOD_PLANKS> {
        static constexpr bool is_visible() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; }
    };
};