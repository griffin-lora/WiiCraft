#pragma once
#include "common.hpp"
#include <gccore.h>
#include <array>
#include <tuple>
#include "math.hpp"

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

    inline void draw_vertex(u8 x, u8 y, u8 z, u8 u, u8 v) {
        GX_Position3u8(x, y, z);
        GX_TexCoord2u8(u, v);
    }

    using texture = GXTexObj;

    std::tuple<bool, error_code> load_from_file(texture& texture, const char* path);
    void safe_load_from_file(texture& texture, const char* path);

    inline void load(texture& texture) {
        GX_LoadTexObj(&texture, GX_TEXMAP0);
    }

    inline void set_filtering_mode(texture& texture, u8 min, u8 mag) {
        GX_InitTexObjFilterMode(&texture, min, mag);
    }

    inline void set_projection_matrix(math::matrix44 matrix, u8 projection_type) {
        GX_LoadProjectionMtx(matrix, projection_type);
    }
    
    inline void set_position_matrix_into_index(math::matrix matrix, u32 index) {
        GX_LoadPosMtxImm(matrix, index);
    }

    inline void set_position_matrix_from_index(u32 index) {
        GX_SetCurrentMtx(index);
    }

    inline void set_channel_count(u8 count) {
        GX_SetNumChans(count);
    }

    inline void set_texture_coordinate_generation_count(u8 count) {
        GX_SetNumTexGens(count);
    }

    inline void init_texture_coordinate_generation(u16 texcoord, u32 tgen_typ, u32 tgen_src, u32 mtxsrc) {
        GX_SetTexCoordGen(texcoord, tgen_typ, tgen_src, mtxsrc);
    }

    inline void set_tev_operation(u8 tevstage, u8 mode) {
        GX_SetTevOp(tevstage, mode);
    }

    inline void set_tev_order(u8 tevstage, u8 texcoord, u32 texmap, u8 color) {
        GX_SetTevOrder(tevstage, texcoord, texmap, color);
    }

    inline void set_z_buffer_mode(bool enable, u8 func, bool update_enable) {
        GX_SetZMode(enable, func, update_enable);
    }
    
    inline void set_color_buffer_update(bool enable) {
        GX_SetColorUpdate(enable);
    }

    inline void copy_framebuffer(void* fb, bool clear) {
        GX_CopyDisp(fb, clear);
    }

    inline void draw_done() {
        GX_DrawDone();
    }
}