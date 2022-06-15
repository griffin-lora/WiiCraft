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
        enum class type : u8 {
            AIR,
            DEBUG,
            GRASS,
            DIRT
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