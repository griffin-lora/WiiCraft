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
        if constexpr (face == block::face::front) {
            return front(std::forward<A>(args)...);
        } else if constexpr (face == block::face::back) {
            return back(std::forward<A>(args)...);
        } else if constexpr (face == block::face::top) {
            return top(std::forward<A>(args)...);
        } else if constexpr (face == block::face::bottom) {
            return bottom(std::forward<A>(args)...);
        } else if constexpr (face == block::face::right) {
            return right(std::forward<A>(args)...);
        } else if constexpr (face == block::face::left) {
            return left(std::forward<A>(args)...);
        }
    }

    template<typename R, typename F, typename ...A>
    constexpr R call_func_on_each_face(F func, A&&... args) {
        func.template operator()<block::face::front>(std::forward<A>(args)...);
        func.template operator()<block::face::back>(std::forward<A>(args)...);
        func.template operator()<block::face::top>(std::forward<A>(args)...);
        func.template operator()<block::face::bottom>(std::forward<A>(args)...);
        func.template operator()<block::face::right>(std::forward<A>(args)...);
        func.template operator()<block::face::left>(std::forward<A>(args)...);
    }
}