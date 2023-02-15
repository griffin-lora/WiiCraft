#include "input.h"

void input_init(u32 width, u32 height) {
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, width, height);
}

vec2s get_dpad_input_vector(u32 buttons_held) {
    vec2s pad_input_vector = {{ 0.0f, 0.0f }};
    if (buttons_held & WPAD_BUTTON_RIGHT) {
        pad_input_vector.x += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_LEFT) {
        pad_input_vector.x -= 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_UP) {
        pad_input_vector.y += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_DOWN) {
        pad_input_vector.y -= 1.0f;
    }
    return pad_input_vector;
}

vec2s get_pointer_position(s32 chan) {
    ir_t pointer;
    WPAD_IR(chan, &pointer);
    if (pointer.valid) {
        return (vec2s){{ pointer.x, pointer.y }};
    } else {
        return (vec2s){{ -1, -1 }};
    }
}

f32 get_plus_minus_input_scalar(u32 buttons_held) {
    f32 scalar = 0.0f;
    if (buttons_held & WPAD_BUTTON_PLUS) {
        scalar += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_MINUS) {
        scalar -= 1.0f;
    }
    return scalar;
}