#pragma once
#include <gctypes.h>
#include <utility>

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
        enum class type : u8 {
            AIR,
            DEBUG,
            GRASS,
            DIRT,
            WOOD_PLANKS
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