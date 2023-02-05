#pragma once
#include "block.hpp"

namespace game {

    template<
        block_face_t FACE,
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
        if constexpr (FACE == block_face_front) {
            return front(std::forward<A>(args)...);
        } else if constexpr (FACE == block_face_back) {
            return back(std::forward<A>(args)...);
        } else if constexpr (FACE == block_face_top) {
            return top(std::forward<A>(args)...);
        } else if constexpr (FACE == block_face_bottom) {
            return bottom(std::forward<A>(args)...);
        } else if constexpr (FACE == block_face_right) {
            return right(std::forward<A>(args)...);
        } else if constexpr (FACE == block_face_left) {
            return left(std::forward<A>(args)...);
        }
    }

    template<typename R, typename F, typename ...A>
    constexpr R call_func_on_each_face(F func, A&&... args) {
        func.template operator()<block_face_front>(std::forward<A>(args)...);
        func.template operator()<block_face_back>(std::forward<A>(args)...);
        func.template operator()<block_face_top>(std::forward<A>(args)...);
        func.template operator()<block_face_bottom>(std::forward<A>(args)...);
        func.template operator()<block_face_right>(std::forward<A>(args)...);
        func.template operator()<block_face_left>(std::forward<A>(args)...);
    }
}