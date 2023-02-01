#pragma once
#include <ogc/gx.h>
#include <stdbool.h>
#include <stddef.h>

extern GXRModeObj* rmode;
extern size_t fb_index;

bool gfx_init(void);
void gfx_update_video(void);