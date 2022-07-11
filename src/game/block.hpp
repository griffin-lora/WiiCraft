#pragma once
#include <gctypes.h>
#include "math.hpp"
#include "ext/data_array.hpp"
#include "common.hpp"
#include <utility>

/**
 * To add a block first, add the type to this list
 * Then add its functionality in block_functionality.hpp
 */
#define EVAL_MACRO_ON_BLOCK_TYPES(macro) \
macro(AIR) \
macro(DEBUG) \
macro(GRASS) \
macro(STONE) \
macro(DIRT) \
macro(WOOD_PLANKS) \
macro(STONE_SLAB) \
macro(TALL_GRASS) \
macro(WATER)

namespace game {
    
    struct block {
        enum class face : u8 {
            FRONT, // +x
            BACK, // -x
            TOP, // +y
            BOTTOM, // -z
            RIGHT, // +z
            LEFT // -z
        };
        
        enum class type : u8 {
            #define EVAL_BLOCK_TYPE_ENUM(T) T,
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_BLOCK_TYPE_ENUM)
        };
        type tp;

        // This is a cache of which faces the block needs to create face vertices for.
        struct face_cache {
            bool front;
            bool back;
            bool top;
            bool bottom;
            bool right;
            bool left;
        };

        enum class slab_state : u8 {
            BOTTOM,
            TOP,
            BOTH
        };

        union state {
            slab_state slab;
        };

        state st;

        inline bool operator!=(const block& other) const {
            return tp != other.tp || st.slab != other.st.slab;
        }

        struct neighborhood_lookup {
            math::vector3u8 position;

            u16 front_index;
            u16 back_index;
            u16 top_index;
            u16 bottom_index;
            u16 right_index;
            u16 left_index;

            bool is_front_edge;
            bool is_back_edge;
            bool is_top_edge;
            bool is_bottom_edge;
            bool is_right_edge;
            bool is_left_edge;
        };

        using neighborhood_lookups = ext::data_array<neighborhood_lookup>;

        struct column_lookup {
            math::vector2u8 position;

            std::array<u16, CHUNK_SIZE> indices;
        };

        using column_lookups = ext::data_array<column_lookup>;
    };

    constexpr u8 block_draw_size = 4;
    constexpr u8 half_block_draw_size = 2;

    // TODO: change this when the sizeof the type is changed
    using bl_st = block::state;
}