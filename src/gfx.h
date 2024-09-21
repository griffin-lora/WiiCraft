#pragma once
#include <ogc/gx.h>
#include <stdbool.h>
#include <stddef.h>

extern GXRModeObj* render_mode;

bool gfx_init(void);
void gfx_update_video(void);