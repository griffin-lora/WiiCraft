#pragma once
#include <gctypes.h>

typedef u32 us_t;

extern us_t total_block_gen_time;
extern us_t total_mesh_gen_time;
extern us_t last_mesh_gen_time;

s64 get_current_us();