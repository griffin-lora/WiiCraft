#include "gfx.h"
#include "log.h"
#include <ogc/tpl.h>
#include <ogc/video.h>
#include <ogc/conf.h>
#include <ogc/system.h>
#include <ogc/cache.h>
#include <string.h>
#include <stdlib.h>

#define NUM_FIFO_BYTES (256 * 1024)

GXRModeObj* render_mode;

static size_t external_framebuffer_index;
static void* external_framebuffers[2];
static bool first_frame = true;

alignas(32) static u8 fifo[NUM_FIFO_BYTES];

bool gfx_init(void) {
    VIDEO_Init();

	render_mode = VIDEO_GetPreferredMode(NULL);
	if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
		render_mode->viWidth = 678;
		render_mode->viXOrigin = (VI_MAX_WIDTH_NTSC - 678) / 2;
	}
	
	VIDEO_Configure(render_mode);
	render_mode = VIDEO_GetPreferredMode(NULL);
	
	lprintf("Video dimensions: %d, %d\nAspect ratio mode: %d\n", render_mode->fbWidth, render_mode->efbHeight, CONF_GetAspectRatio());

	size_t num_external_framebuffer_bytes = VIDEO_GetFrameBufferSize(render_mode);
	for (size_t i = 0; i < 2; i++) {
		external_framebuffers[i] = SYS_AllocateFramebuffer(render_mode);
		DCInvalidateRange(external_framebuffers[i], num_external_framebuffer_bytes);
		external_framebuffers[i] = MEM_K0_TO_K1(external_framebuffers[i]);
	}

	lprintf("Num external framebuffer bytes: %d\n", num_external_framebuffer_bytes);

	memset(fifo, 0, NUM_FIFO_BYTES);
	VIDEO_SetNextFramebuffer(external_framebuffers[external_framebuffer_index]);
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (render_mode->viTVMode & VI_NON_INTERLACE) {
		VIDEO_WaitVSync();
	}

	external_framebuffer_index ^= 1;

	GX_Init(fifo, NUM_FIFO_BYTES);

	GX_SetCopyClear((GXColor) {0, 0, 0, 0xff}, 0x00ffffff);

	GX_SetViewport(0, 0, render_mode->fbWidth, render_mode->efbHeight, 0, 1);
	u32 external_frame_buffer_height = GX_SetDispCopyYScale(GX_GetYScaleFactor(render_mode->efbHeight, render_mode->xfbHeight));
	GX_SetScissor(0, 0, render_mode->fbWidth, render_mode->efbHeight);
	GX_SetDispCopySrc(0, 0, render_mode->fbWidth, render_mode->efbHeight);
	GX_SetDispCopyDst(render_mode->fbWidth, (u16) external_frame_buffer_height);
	GX_SetCopyFilter(render_mode->aa, render_mode->sample_pattern, GX_TRUE, render_mode->vfilter);
	GX_SetFieldMode(render_mode->field_rendering, ((render_mode->viHeight == 2*render_mode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

	if (render_mode->aa) {
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	} else {
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	GX_CopyDisp(external_framebuffers[external_framebuffer_index], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_InvVtxCache();
	GX_InvalidateTexAll();
	
	GX_ClearVtxDesc();

	return true;
}

void gfx_update_video() {
    GX_CopyDisp(external_framebuffers[external_framebuffer_index], GX_TRUE);

    GX_DrawDone();

    VIDEO_SetNextFramebuffer(external_framebuffers[external_framebuffer_index]);
    if (first_frame) {
        first_frame = false;
        VIDEO_SetBlack(FALSE);
    }
    VIDEO_Flush();
    VIDEO_WaitVSync();
    external_framebuffer_index ^= 1;
}