#pragma once
#include <gctypes.h>
#include <utility>

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
        
        // TODO: Fix this mess
        /**
         * To add a block type:
         * Add it to the enum
         * Add it to the is_block_visible function
         * Add it to the get_face_vertex_count function
         * Add it to the add_face_vertices function
         */
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