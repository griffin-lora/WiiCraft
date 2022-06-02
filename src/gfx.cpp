#include "gfx.hpp"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <array>

static void enable_console() {
    // Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	GXRModeObj* rmode = VIDEO_GetPreferredMode(nullptr);

	// Allocate memory for the display in the uncached region
	void* xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// Initialise the console, required for printf
	CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) {
		VIDEO_WaitVSync();
	}

	std::printf("\x1b[2;0H");
}

constexpr std::size_t DEFAULT_FIFO_SIZE = 256 * 1024;
std::array<void*, 2> fbs = { nullptr, nullptr };

static void enable_gx() {
	u32 fb_index = 0;
	f32 y_scale = 0;
	f32 zt = 0;
	u32 xfb_height;

	GXColor background = {0, 0, 0, 0xff};
	GXRModeObj* rmode = VIDEO_GetPreferredMode(nullptr);

	// allocate the fifo buffer
	void* gpfifo = memalign(32, DEFAULT_FIFO_SIZE);
	std::memset(gpfifo, 0, DEFAULT_FIFO_SIZE);

	// allocate 2 framebuffers for double buffering
	fbs[0] = SYS_AllocateFramebuffer(rmode);
	fbs[1] = SYS_AllocateFramebuffer(rmode);

	// configure video
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(fbs[fb_index]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb_index ^= 1;

	// init the flipper
	GX_Init(gpfifo,DEFAULT_FIFO_SIZE);

	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);

	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	y_scale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfb_height = GX_SetDispCopyYScale(y_scale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfb_height);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(fbs[fb_index],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);
}

static gfx_mode cur_mode = gfx_mode::none;

void enable_gfx_mode(gfx_mode mode) {
    if (mode == cur_mode) { return; }
    cur_mode = mode;
    if (cur_mode == gfx_mode::console) {
        enable_console();
    } else {
        enable_gx();
    }
}