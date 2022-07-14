#pragma once
#include "gfx/text.hpp"
#include <string>

namespace game {
    struct debug_ui {
        gfx::text fps_text;

        void update(u32 fps) {
            fps_text.update("FPS: " + std::to_string(fps), 16, 16);
        }
        void draw() {
            fps_text.draw();
        }
    };
};