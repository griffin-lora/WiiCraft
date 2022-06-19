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
macro(WOOD_PLANKS)

namespace game {
    
    struct block {
        enum class face : u8 {
            FRONT, // +x
            BACK, // -x
            TOP, // +y
            BOTTOM, // -z
            RIGHT, // +z
            LEFT, // -z
            CENTER
        };
        
        #define EVAL_BLOCK_TYPE_ENUM(name) name,
        enum class type : u8 {
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
    };
};