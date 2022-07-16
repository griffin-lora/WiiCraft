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

    inline bool scan_nunchuk(s32 chan, expansion_t& exp) {
        WPAD_Expansion(chan, &exp);
        return exp.type == WPAD_EXP_NUNCHUK;
    }

    inline u32 get_buttons_held(s32 chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline u32 get_buttons_down(s32 chan) {
        return WPAD_ButtonsDown(chan);
    }

    inline glm::vec3 get_gforce(s32 chan) {
        glm::vec3 gforce;
        WPAD_GForce(0, (gforce_t*)&gforce);
        return gforce;
    }

    inline math::vector3u16 get_accel(s32 chan) {
        math::vector3u16 accel;
        WPAD_Accel(chan, (vec3w_t*)&accel);
        return accel;
    }

    inline glm::vec2 get_nunchuk_vector(const nunchuk_t& nunchuk) {
        return { nunchuk.js.pos.x - nunchuk.js.center.x, nunchuk.js.pos.y - nunchuk.js.center.y };
    }

    glm::vec2 get_dpad_input_vector(u32 buttons_held);
    std::optional<glm::vec2> get_pointer_position();
    f32 get_plus_minus_input_scalar(u32 buttons_held);
}