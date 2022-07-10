#pragma once
#include <gctypes.h>
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
    };

    constexpr u8 block_draw_size = 4;
    constexpr u8 half_block_draw_size = 2;

    // TODO: change this when the sizeof the type is changed
    using bl_st = block::state;
}