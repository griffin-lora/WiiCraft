#pragma once
#include "block.hpp"

namespace game {

    template<
        block::face face,
        typename R,
        typename F0,
        typename F1,
        typename F2,
        typename F3,
        typename F4,
        typename F5,
        typename ...A
    >
    constexpr R call_face_func_for(F0 front, F1 back, F2 top, F3 bottom, F4 right, F5 left, A&&... args) {
        if constexpr (face == block::face::FRONT) {
            return front(std::forward<A>(args)...);
        } else if constexpr (face == block::face::BACK) {
            return back(std::forward<A>(args)...);
        } else if constexpr (face == block::face::TOP) {
            return top(std::forward<A>(args)...);
        } else if constexpr (face == block::face::BOTTOM) {
            return bottom(std::forward<A>(args)...);
        } else if constexpr (face == block::face::RIGHT) {
            return right(std::forward<A>(args)...);
        } else if constexpr (face == block::face::LEFT) {
            return left(std::forward<A>(args)...);
        }
    }

    template<typename R, typename F, typename ...A>
    constexpr R call_func_on_each_face(F func, A&&... args) {
        func.template operator()<block::face::FRONT>(std::forward<A>(args)...);
        func.template operator()<block::face::BACK>(std::forward<A>(args)...);
        func.template operator()<block::face::TOP>(std::forward<A>(args)...);
        func.template operator()<block::face::BOTTOM>(std::forward<A>(args)...);
        func.template operator()<block::face::RIGHT>(std::forward<A>(args)...);
        func.template operator()<block::face::LEFT>(std::forward<A>(args)...);
    }
}