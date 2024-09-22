#pragma once
#include "chrono.h"
#include <gctypes.h>

void debug_ui_init(void);
void debug_ui_update(u32 buttons_down);
void debug_ui_draw(us_t total_procedural_gen_time, us_t total_visual_gen_time, us_t last_visual_gen_time, u32 fps);