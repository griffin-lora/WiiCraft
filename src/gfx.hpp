#pragma once
#include <gccore.h>
#include <array>

namespace gfx {
    struct color3 {
        u8 r;
        u8 g;
        u8 b;
    };

    using color4 = GXColor;

    struct console_state {
        GXRModeObj* rmode = nullptr;
        void* xfb = nullptr;
    };

    void init(console_state& state);

    struct draw_state {
        GXRModeObj* rmode = nullptr;
        u32 fb = 0;
        std::array<void*, 2> frameBuffer{{ NULL, NULL }};
        void* gpfifo;
    };

    void init(draw_state& state, color4 bkg);

    inline void set_vert_color(const color3& color) {
        GX_Color3u8(color.r, color.g, color.b);
    }
}