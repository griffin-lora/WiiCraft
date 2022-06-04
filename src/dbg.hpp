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

    template<typename Tf, typename Cf, typename ...Args>
    inline void try_catch(Tf try_func, Cf catch_func, Args&& ...args) {
        auto [ success, code ] = try_func(std::forward<Args>(args)...);
        if (!success) {
            gfx::console_state console;
            gfx::init(console);
            catch_func(code, std::forward<Args>(args)...);
            freeze();
        }
    }
};