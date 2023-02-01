#pragma once

namespace dbg {
    void freeze();

    template<typename F>
    inline void error(F func) {
    }

    template<typename F1, typename F2, typename ...Args>
    inline void try_catch(const F1& try_func, const F2& catch_func, Args&& ...args) {
        
    }

    struct non_copyable {
        non_copyable() = default;
        non_copyable(const non_copyable&) = delete;
        non_copyable& operator=(const non_copyable&) = delete;
        non_copyable(non_copyable&&) {}
        non_copyable& operator=(non_copyable&&) { return *this; }
    };
}