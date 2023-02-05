#pragma once
#include <stddef.h>

typedef struct {
    u16 size;
    void* data;
} display_list_t;

typedef struct {
    u16 size;
    u16 chunk_index;
} pool_display_list_t;