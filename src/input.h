#pragma once
#include "game_math.h"
#include <ogc/lwp_queue.h>
#include <wiiuse/wpad.h>

void input_init(u32 width, u32 height);

inline vec2s get_nunchuk_vector(const nunchuk_t* nunchuk) {
    return (vec2s){ .x = (f32)(nunchuk->js.pos.x - nunchuk->js.center.x), .y = (f32)(nunchuk->js.pos.y - nunchuk->js.center.y) };
}

vec2s get_dpad_input_vector(u32 buttons_held);
vec2s get_pointer_position(s32 chan);
f32 get_plus_minus_input_scalar(u32 buttons_held);