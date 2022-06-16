#include "input.hpp"

using namespace input;

void input::init(u32 width, u32 height) {
    // WPAD_Init();
    // WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    // WPAD_SetVRes(WPAD_CHAN_ALL, width, height);
}

glm::vec2 input::get_dpad_input_vector(u32 buttons_held) {
    glm::vec2 pad_input_vector = {0.0f, 0.0f};
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

glm::vec2 input::get_pointer_input_vector(state& s, u32 buttons_held) {
    // ir_t pointer;
    // WPAD_IR(0, &pointer);
    // if (pointer.valid) {
    //     glm::vec2 pointer_pos = {pointer.sx, pointer.sy};
    //     if ((buttons_held & WPAD_BUTTON_A) && s.was_last_pointer_pos_valid && pointer_pos != s.last_pointer_pos) {
    //         glm::vec2 pointer_input_vector = pointer_pos - s.last_pointer_pos;
    //         s.was_last_pointer_pos_valid = true;
    //         s.last_pointer_pos = pointer_pos;
    //         return pointer_input_vector;
    //     }
    //     s.was_last_pointer_pos_valid = true;
    //     s.last_pointer_pos = pointer_pos;
    // } else {
    //     s.was_last_pointer_pos_valid = false;
    // }
    return {0.0f, 0.0f};
}

glm::vec2 input::get_joystick_input_vector() {
    // expansion_t exp;
    // WPAD_Expansion(0, &exp);

    // if (exp.type == WPAD_EXP_NUNCHUK) {
    //     return { exp.nunchuk.js.pos.x - exp.nunchuk.js.center.x, exp.nunchuk.js.pos.y - exp.nunchuk.js.center.y };
    // }
    return { 0.0f, 0.0f };
}

float input::get_plus_minus_input_scalar(u32 buttons_held) {
    float scalar = 0.0f;
    if (buttons_held & WPAD_BUTTON_PLUS) {
        scalar += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_MINUS) {
        scalar -= 1.0f;
    }
    return scalar;
}