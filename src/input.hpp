#pragma once
#include "math.hpp"
#include <wiiuse/wpad.h>

namespace input {
    struct state {
        bool was_last_pointer_pos_valid = false;
	    math::vector2f last_pointer_pos = {0.0f, 0.0f};
    };

    void init(u32 width, u32 height);

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(int chan) {
        return WPAD_ButtonsHeld(chan);
    }

    math::vector3f get_dpad_input_vector(u32 buttons_held);
    math::vector2f get_pointer_input_vector(state& s, u32 buttons_held);
}