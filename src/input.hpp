#pragma once
#include "math.hpp"
#include <wiiuse/wpad.h>
#include <optional>

namespace input {
    struct state {
    };

    void init(u32 width, u32 height);

    inline void set_resolution(u32 width, u32 height) {
        WPAD_SetVRes(WPAD_CHAN_ALL, width, height);
    }

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(s32 chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline u32 get_buttons_down(s32 chan) {
        return WPAD_ButtonsDown(chan);
    }

    glm::vec2 get_dpad_input_vector(u32 buttons_held);
    std::optional<glm::vec2> get_pointer_position();
    glm::vec2 get_joystick_input_vector();
    f32 get_plus_minus_input_scalar(u32 buttons_held);
}