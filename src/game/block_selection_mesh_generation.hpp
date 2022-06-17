#pragma once
#include <gccore.h>

namespace game {
    struct block_selection_vert_func {
        static inline void call(u8 x, u8 y, u8 z, u8, u8) {
            GX_Position3u8(x, y, z);
            GX_Color4u8(0xff, 0xff, 0xff, 0x7f);
        }
    };
};