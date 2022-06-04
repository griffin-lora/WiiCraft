#pragma once
#include "gfx.hpp"

namespace dbg {
    void freeze();

    template<typename F>
    inline void error(F func) {
        gfx::console_state console;
        gfx::init(console);
        func();
        freeze();
    }
};