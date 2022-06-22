#include <ogc/video.h>

void VIDEO_Init(void) {}

static GXRModeObj rmode;

GXRModeObj* VIDEO_GetPreferredMode(GXRModeObj *mode) {
    return mode;
}
void VIDEO_Configure(GXRModeObj *rmode) {}
void VIDEO_SetNextFramebuffer(void *fb) {}
void VIDEO_SetBlack(bool black) {}
void VIDEO_Flush(void) {}

void VIDEO_WaitVSync(void) {}