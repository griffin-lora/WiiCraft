#pragma once
#include <gccore.h>

namespace gfx {
    struct console_state {
        void* xfb = nullptr;
        GXRModeObj* rmode = nullptr;
    };

    void init(console_state& state);

    struct draw_state {
        GXRModeObj* rmode;
    };

    void init(draw_state& state);

    struct color {
        u8 r;
        u8 g;
        u8 b;
    };

    inline void set_vert_color(const color& color) {
        GX_Color3u8(color.r, color.g, color.b);
    }
}