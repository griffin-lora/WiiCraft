#pragma once
#include "block.hpp"

namespace game {
    template<block::type type>
    struct block_type {
        static inline bool is_visible() { return false; }
    };

    template<>
    struct block_type<block::type::DEBUG> {
        static inline bool is_visible() { return true; }
    };

    template<>
    struct block_type<block::type::GRASS> {
        static inline bool is_visible() { return true; }
    };

    template<>
    struct block_type<block::type::STONE> {
        static inline bool is_visible() { return true; }
    };

    template<>
    struct block_type<block::type::DIRT> {
        static inline bool is_visible() { return true; }
    };

    template<>
    struct block_type<block::type::WOOD_PLANKS> {
        static inline bool is_visible() { return true; }
    };
};