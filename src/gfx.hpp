#pragma once
#include <gccore.h>
#include <unistd.h>
#include <string>

namespace gfx {
    enum class mode {
        console,
        gx,
        none
    };

    void enable_mode(mode mode);

    class texture {
        GXTexObj tex_obj;
        public:
            bool load_from_file(const std::string& path);
            void use();
    };

    inline void wait_for_vsync() {
        VIDEO_WaitVSync();
    }
}