#pragma once
#include "math.hpp"
#include <wiiuse/wpad.h>

namespace input {
    struct state {
        bool was_last_pointer_pos_valid = false;
	    glm::vec2 last_pointer_pos = {0.0f, 0.0f};
    };

    void init(u32 width, u32 height);

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(int chan) {
        return WPAD_ButtonsHeld(chan);
    }

    glm::vec2 get_dpad_input_vector(u32 buttons_held);
    glm::vec2 get_pointer_input_vector(state& s, u32 buttons_held);
    glm::vec2 get_joystick_input_vector();
    float get_plus_minus_input_scalar(u32 buttons_held);
}