#pragma once
#include <stddef.h>

inline size_t align_to_32(size_t n) {
    return (n | 31) + 33;
}