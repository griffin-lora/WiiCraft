#pragma once
#include "math.hpp"
#include "game/camera.hpp"
#include "game/block_logic.hpp"
#include "game/cursor.hpp"
#include <wiiuse/wpad.h>

namespace input {
    struct state {
    };

    void init(u32 width, u32 height);

    void handle(f32 cam_movement_speed, f32 cam_rotation_speed, game::camera& cam, game::cursor& cursor, std::optional<game::raycast>& raycast);

    inline void scan_pads() {
        WPAD_ScanPads();
    }

    inline u32 get_buttons_held(int chan) {
        return WPAD_ButtonsHeld(chan);
    }

    inline u32 get_buttons_down(int chan) {
        return WPAD_ButtonsDown(chan);
    }
}