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

    inline bool scan_nunchuk(expansion_t& exp) {
        WPAD_Expansion(0, &exp);
        return exp.type == WPAD_EXP_NUNCHUK;
    }

    inline u32 get_buttons_held(s32 chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline u32 get_buttons_down(s32 chan) {
        return WPAD_ButtonsDown(chan);
    }

    inline glm::vec2 get_nunchuk_vector(const nunchuk_t& nunchuk) {
        return { nunchuk.js.pos.x - nunchuk.js.center.x, nunchuk.js.pos.y - nunchuk.js.center.y };
    }

    glm::vec2 get_dpad_input_vector(u32 buttons_held);
    std::optional<glm::vec2> get_pointer_position();
    f32 get_plus_minus_input_scalar(u32 buttons_held);
}