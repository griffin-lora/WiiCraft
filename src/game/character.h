#pragma once
#include "game_math.h"
#include "chrono.h"
#include "input.h"
#include <stdbool.h>

extern vec3s character_position;
extern vec3s character_velocity;

extern bool grounded;
extern bool sprinting;

void character_handle_input(vec3w_t last_wpad_accel, vec3w_t last_nunchuk_accel, us_t now, f32 delta, vec3w_t wpad_accel, vec2s nunchuk_vector, u8 nunchuk_buttons_down, vec3w_t nunchuk_accel);

void character_apply_physics(s32vec3s region_pos, f32 delta);
void character_apply_velocity(f32 delta);