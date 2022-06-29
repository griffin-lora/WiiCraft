#include "gfx.hpp"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <unistd.h>
#include <cerrno>
#include "dbg.hpp"

using namespace gfx;

console_state::console_state() {
    // Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	#ifndef PC_PORT
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	#else
	xfb = (void*)0x0;
	#endif

	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);

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
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	#ifndef PC_PORT
	std::printf("\x1b[2;0H");
	#endif
}

constexpr std::size_t DEFAULT_FIFO_SIZE = 256*1024;

draw_state::draw_state(color4 bkg) {

	VIDEO_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	// allocate the fifo buffer
	gpfifo = memalign(32,DEFAULT_FIFO_SIZE);
	std::memset(gpfifo,0,DEFAULT_FIFO_SIZE);

	// allocate 2 framebuffers for double buffering
	frame_buffers[0] = SYS_AllocateFramebuffer(rmode);
	frame_buffers[1] = SYS_AllocateFramebuffer(rmode);

	// configure video
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frame_buffers[fb_index]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb_index ^= 1;

	// init the flipper
	GX_Init(gpfifo,DEFAULT_FIFO_SIZE);

	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(bkg, 0x00ffffff);

	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_CopyDisp(frame_buffers[fb_index],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_InvVtxCache();
	GX_InvalidateTexAll();
}

std::tuple<bool, error_code> gfx::load_from_file(texture& tex, const char* path) {
	#ifndef PC_PORT
	if (access(path, R_OK) != 0) {
		return { false, errno };
	}
	#endif
	TPLFile tpl_file;
	TPL_OpenTPLFromFile(&tpl_file, path);
	TPL_GetTexture(&tpl_file, 0, &tex);
	return { true, 0 };
}

void gfx::safe_load_from_file(texture& tex, const char* path) {
	dbg::try_catch([](auto& tex, auto path) {
		return gfx::load_from_file(tex, path);
	}, [](auto code, auto& _, auto path) {
		std::printf("Failed to load texture from \"%s\", error code: %d\n", path, code);
	}, tex, path);
}