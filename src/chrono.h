#pragma once
#include <gctypes.h>

typedef u32 us_t;

extern us_t total_procedural_gen_time;
extern us_t total_visual_gen_time;
extern us_t last_visual_gen_time;

s64 get_current_us();