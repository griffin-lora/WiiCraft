#pragma once
#include <stddef.h>

#define ALIGN_TO_32(N) (((N) | 31) + 33)
#define ALIGN_TO_32_NEW(N) (((N) + 32) - ((N) % 32))

inline size_t align_to_32(size_t n) {
    return ALIGN_TO_32(n);
}

inline s32 mod_s32(s32 a, s32 b) {
    s32 ret = a % b;
    return ret >= 0 ? ret : ret + b;
}