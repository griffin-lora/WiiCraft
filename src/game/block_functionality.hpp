#pragma once
#include "block_functionality_core.hpp"

namespace game {
    template<typename F>
    inline void call_with_block_functionality(block::type tp, F func) {
        #define EVAL_CALL_WITH_BLOCK_FUNCTIONALITY_CASE(T) case block::type::T: \
            func.template operator()<block_functionality<block::type::T>>(); \
            return;

        switch (tp) {
            default: return;
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_CALL_WITH_BLOCK_FUNCTIONALITY_CASE)
        }
    }

    template<typename R, typename F>
    inline R get_with_block_functionality(block::type tp, F func) {
        #define EVAL_GET_WITH_BLOCK_FUNCTIONALITY_CASE(T) case block::type::T: \
            return func.template operator()<block_functionality<block::type::T>>();

        switch (tp) {
            default: return {};
            EVAL_MACRO_ON_BLOCK_TYPES(EVAL_GET_WITH_BLOCK_FUNCTIONALITY_CASE)
        }
    }
};