#pragma once
#include <gccore.h>

namespace gfx {
    struct color {
        u8 r;
        u8 g;
        u8 b;
    };

    inline void set_vert_color(const color& c) {
        GX_Color3u8(c.r, c.g, c.b);
    }
}