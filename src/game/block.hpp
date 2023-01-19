#pragma once
#include <gctypes.h>
#include "math.hpp"
#include "ext/data_array.hpp"
#include <utility>

/**
 * To add a block first, add the type to this list
 * Then add its functionality in block_functionality.hpp
 */
#define EVAL_MACRO_ON_BLOCK_TYPES(macro) \
macro(air) \
macro(debug) \
macro(grass) \
macro(stone) \
macro(dirt) \
macro(sand) \
macro(wood_planks) \
macro(stone_slab_bottom) \
macro(stone_slab_top) \
macro(stone_slab_both) \
macro(tall_grass) \
macro(water)

namespace game {
    
    struct block {
        enum class face : u8 {
            front, // +x
            back, // -x
            top, // +y
            bottom, // -z
            right, // +z
            left // -z
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
            bottom,
            top,
            both
        };

        union state {
            slab_state slab;
        };

        inline bool operator!=(const block& other) const {
            return tp != other.tp;
        }


        enum class category {
            transparent,
            opaque_cube,
            transparent_cube,
            opaque_top_slab,
            opaque_bottom_slab,
        };
    };

    constexpr u8 block_draw_size = 4;
    constexpr u8 half_block_draw_size = 2;

    // TODO: change this when the sizeof the type is changed
    using bl_st = block::state;

    static_assert(sizeof(block) == 1, "block must be 1 byte wide");
}