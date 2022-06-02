#pragma once
#include <gccore.h>

enum class gfx_mode {
    console,
    gx,
    none
};

void enable_gfx_mode(gfx_mode mode);

inline void wait_for_vsync() {
    VIDEO_WaitVSync();
}