#include "gfx.h"
#ifndef PC_PORT
#include "../build/textures_tpl.h"
#include "../build/textures.h"
#endif
#include <ogc/tpl.h>
#include <ogc/video.h>
#include <string.h>

#define DEFAULT_FIFO_SIZE (256 * 1024)
#define DEFAULT_FRAMEBUFFER_SIZE 614400

GXRModeObj* rmode;
size_t fb_index;

_Alignas(32) static struct {
	u8 frame_buffers[2][DEFAULT_FRAMEBUFFER_SIZE];
	_Alignas(32) u8 gpfifo[DEFAULT_FIFO_SIZE];
} video;
static bool first_frame = true;

_Alignas(32) static struct {
	GXTexObj chunk;
	GXTexObj icons;
	GXTexObj skybox;
	GXTexObj font;
} textures;

bool gfx_init(void) {
    VIDEO_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

    if (VIDEO_GetFrameBufferSize(rmode) != DEFAULT_FRAMEBUFFER_SIZE) {
		return false;
	}

	// set the fifo buffer to 0
	memset(video.gpfifo, 0, DEFAULT_FIFO_SIZE);

	// configure video
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(video.frame_buffers[fb_index]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb_index ^= 1;

	// init the (not flipper)
	GX_Init(video.gpfifo, DEFAULT_FIFO_SIZE);

	// clears the bg to color and clears the z buffer
	GX_SetCopyClear((GXColor){0, 0, 0, 0xff}, 0x00ffffff);

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

	GX_CopyDisp(video.frame_buffers[fb_index],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_InvVtxCache();
	GX_InvalidateTexAll();

    #ifndef PC_PORT
    TPLFile file;
    TPL_OpenTPLFromMemory(&file, (void*)textures_tpl, textures_tpl_size);

    TPL_GetTexture(&file, chunk_tex, &textures.chunk);
    TPL_GetTexture(&file, icons_tex, &textures.icons);
    TPL_GetTexture(&file, skybox_tex, &textures.skybox);
    TPL_GetTexture(&file, font_tex, &textures.font);
    #endif

	GX_InitTexObjFilterMode(&textures.chunk, GX_NEAR, GX_NEAR);
	GX_InitTexObjFilterMode(&textures.icons, GX_NEAR, GX_NEAR);
	GX_InitTexObjFilterMode(&textures.font, GX_NEAR, GX_NEAR);

	GX_LoadTexObj(&textures.chunk, GX_TEXMAP0);
	GX_LoadTexObj(&textures.icons, GX_TEXMAP1);
	GX_LoadTexObj(&textures.skybox, GX_TEXMAP2);
	GX_LoadTexObj(&textures.font, GX_TEXMAP3);

	return true;
}

void gfx_update_video() {
    GX_CopyDisp(video.frame_buffers[fb_index], true);

    GX_DrawDone();

    VIDEO_SetNextFramebuffer(video.frame_buffers[fb_index]);
    if (first_frame) {
        first_frame = false;
        VIDEO_SetBlack(FALSE);
    }
    VIDEO_Flush();
    VIDEO_WaitVSync();
    fb_index ^= 1;
}