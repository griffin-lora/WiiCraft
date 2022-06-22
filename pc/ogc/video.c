#include <ogc/video.h>
#include <unistd.h>

void VIDEO_Init(void) {}

static GXRModeObj rmode;

GXRModeObj* VIDEO_GetPreferredMode(GXRModeObj *mode) {
    return &rmode;
}
void VIDEO_Configure(GXRModeObj *rmode) {}
void VIDEO_SetNextFramebuffer(void *fb) {}
void VIDEO_SetBlack(bool black) {}
void VIDEO_Flush(void) {}

void VIDEO_WaitVSync(void) {
    usleep(16666);
}