#pragma once
#include <stddef.h>
#include <gctypes.h>

constexpr size_t get_begin_instruction_size(u16 vert_count) {
    return vert_count > 0xff ? 4 : 3;
}

template<typename T>
constexpr size_t get_vector_instruction_size(size_t dim, u16 vert_count) {
    return vert_count * dim * sizeof(T);
}