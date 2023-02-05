#pragma once
#include <stddef.h>

#define ALIGN_TO_32(N) (((N) | 31) + 33)

inline size_t align_to_32(size_t n) {
    return ALIGN_TO_32(n);
}