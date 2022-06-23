#pragma once
#include <gctypes.h>
#include <utility>

#define X(...) __VA_ARGS__

/**
 * To add a block first, add the type to this list
 * Then add its functionality in block_functionality.hpp
 */
#define EVAL_MACRO_ON_BLOCK_TYPES(macro, param1) \
macro(AIR, X(param1)) \
macro(DEBUG, X(param1)) \
macro(GRASS, X(param1)) \
macro(STONE, X(param1)) \
macro(DIRT, X(param1)) \
macro(WOOD_PLANKS, X(param1)) \
macro(STONE_SLAB, X(param1))

#define EVAL_BLOCK_FUNCTIONALITY_CASE(tp, case_body) case block::type::tp: { using Bf = block_functionality<block::type::tp>; case_body }

#define EVAL_BLOCK_FUNCTIONALITY_CASES(case_body) EVAL_MACRO_ON_BLOCK_TYPES(EVAL_BLOCK_FUNCTIONALITY_CASE, X(case_body))

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
            #define EVAL_BLOCK_TYPE_ENUM(tp, x) tp,
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_BLOCK_TYPE_ENUM, x)
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
    };
};