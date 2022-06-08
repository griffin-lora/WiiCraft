#pragma once
#include "math.hpp"
#include <wiiuse/wpad.h>

namespace input {
    struct state {
        bool was_last_pointer_pos_valid = false;
	    math::vector2f last_pointer_pos = {0.0f, 0.0f};
    };

    void init(u32 width, u32 height) {
        WPAD_Init();
        WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
        WPAD_SetVRes(WPAD_CHAN_ALL, width, height);
    }

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(int chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline math::vector3f get_dpad_input_vector(u32 buttons_held) {
        math::vector3f pad_input_vector = {0.0f, 0.0f, 0.0f};
        if (buttons_held & WPAD_BUTTON_UP) {
			pad_input_vector += { 1, 0, 0 };
		}
		if (buttons_held & WPAD_BUTTON_DOWN) {
			pad_input_vector -= { 1, 0, 0 };
		}
		if (buttons_held & WPAD_BUTTON_LEFT) {
			pad_input_vector += { 0, 0, 1 };
		}
		if (buttons_held & WPAD_BUTTON_RIGHT) {
			pad_input_vector -= { 0, 0, 1 };
		}
		if (buttons_held & WPAD_BUTTON_PLUS) {
			pad_input_vector += { 0, 1, 0 };
		}
		if (buttons_held & WPAD_BUTTON_MINUS) {
			pad_input_vector -= { 0, 1, 0 };
		}
        return pad_input_vector;
    }

    inline math::vector2f get_pointer_input_vector(state& s, u32 buttons_held) {
        ir_t pointer;
		WPAD_IR(0, &pointer);
		if (pointer.valid) {
			math::vector2f pointer_pos = {pointer.x, pointer.y};
			if ((buttons_held & WPAD_BUTTON_A) && s.was_last_pointer_pos_valid && pointer_pos != s.last_pointer_pos) {
				math::vector2f pointer_input_vector = pointer_pos - s.last_pointer_pos;
                s.was_last_pointer_pos_valid = true;
                s.last_pointer_pos = pointer_pos;
                return pointer_input_vector;
			}
			s.was_last_pointer_pos_valid = true;
			s.last_pointer_pos = pointer_pos;
		} else {
			s.was_last_pointer_pos_valid = false;
		}
        return {0.0f, 0.0f};
    }
}