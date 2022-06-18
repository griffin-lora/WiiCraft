#pragma once
#include "math.hpp"
#include <wiiuse/wpad.h>
#include <optional>

namespace input {
    struct state {
    };

    void init(u32 width, u32 height);

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(int chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline u32 get_buttons_down(int chan) {
        return WPAD_ButtonsDown(chan);
    }

    glm::vec2 get_dpad_input_vector(u32 buttons_held);
    std::optional<glm::vec2> get_pointer_position();
    glm::vec2 get_joystick_input_vector();
    float get_plus_minus_input_scalar(u32 buttons_held);
}