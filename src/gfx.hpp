#pragma once
#include "common.hpp"
#include <gccore.h>
#include <array>
#include <tuple>

namespace gfx {

    using color4 = GXColor;

    struct console_state {
        GXRModeObj* rmode = nullptr;
        void* xfb = nullptr;
    };

    void init(console_state& state);

    struct draw_state {
        GXRModeObj* rmode = nullptr;
        u32 fb_index = 0;
        std::array<void*, 2> frame_buffers{{ NULL, NULL }};
        void* gpfifo;
    };

    void init(draw_state& state, color4 bkg);

    template<typename F>
    inline void draw_quads(std::size_t vertices, F func) {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertices);
        func();
        GX_End();
    }

    inline void draw_vertex(f32 x, f32 y, f32 z, f32 u, f32 v) {
        GX_Position3f32(x, y, z);
        GX_TexCoord2f32(u, v);
    }

    using texture = GXTexObj;

    std::tuple<bool, error_code> load_from_file(texture& texture, const char* path);
    void safe_load_from_file(texture& texture, const char* path);

    inline void set_filtering_mode(texture& texture, u8 min, u8 mag) {
        GX_InitTexObjFilterMode(&texture, min, mag);
    }
}