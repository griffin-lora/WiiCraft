#pragma once
#include "math.hpp"
#include "game/camera.hpp"
#include "game/block_logic.hpp"
#include <wiiuse/wpad.h>

namespace input {
    struct state {
        bool was_last_pointer_pos_valid = false;
	    glm::vec2 last_pointer_pos = {0.0f, 0.0f};
    };

    void init(u32 width, u32 height);

    void handle(f32 cam_movement_speed, f32 cam_rotation_speed, game::camera& cam, std::optional<game::raycast>& raycast);

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