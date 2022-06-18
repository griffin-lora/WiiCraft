#include "gfx.hpp"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <unistd.h>
#include <cerrno>
#include "dbg.hpp"

using namespace gfx;

void gfx::init(console_state& s) {
    // Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	s.rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	s.xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(s.rmode));

	// Initialise the console, required for printf
	console_init(s.xfb,20,20,s.rmode->fbWidth,s.rmode->xfbHeight,s.rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(s.rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(s.xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(s.rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	std::printf("\x1b[2;0H");
}

constexpr std::size_t DEFAULT_FIFO_SIZE = 256*1024;

void gfx::init(draw_state& s, color4 bkg) {

	VIDEO_Init();

	s.rmode = VIDEO_GetPreferredMode(NULL);

	// allocate the fifo buffer
	s.gpfifo = memalign(32,DEFAULT_FIFO_SIZE);
	std::memset(s.gpfifo,0,DEFAULT_FIFO_SIZE);

	// allocate 2 framebuffers for double buffering
	s.frame_buffers[0] = SYS_AllocateFramebuffer(s.rmode);
	s.frame_buffers[1] = SYS_AllocateFramebuffer(s.rmode);

	// configure video
	VIDEO_Configure(s.rmode);
	VIDEO_SetNextFramebuffer(s.frame_buffers[s.fb_index]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(s.rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	s.fb_index ^= 1;

	// init the flipper
	GX_Init(s.gpfifo,DEFAULT_FIFO_SIZE);

	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(bkg, 0x00ffffff);

	// other gx setup
	GX_SetViewport(0,0,s.rmode->fbWidth,s.rmode->efbHeight,0,1);
	f32 yscale = GX_GetYScaleFactor(s.rmode->efbHeight,s.rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,s.rmode->fbWidth,s.rmode->efbHeight);
	GX_SetDispCopySrc(0,0,s.rmode->fbWidth,s.rmode->efbHeight);
	GX_SetDispCopyDst(s.rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(s.rmode->aa,s.rmode->sample_pattern,GX_TRUE,s.rmode->vfilter);
	GX_SetFieldMode(s.rmode->field_rendering,((s.rmode->viHeight==2*s.rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (s.rmode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(s.frame_buffers[s.fb_index],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// GX_VTXFMT0 is for standard cube geometry
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 0);
	// Since the fractional size of the fixed point number is 4, it is equivalent to 1 unit = 16 pixels
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);

	GX_InvVtxCache();
	GX_InvalidateTexAll();
}

std::tuple<bool, error_code> gfx::load_from_file(texture& tex, const char* path) {
	if (access(path, R_OK) != 0) {
		return { false, errno };
	}
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