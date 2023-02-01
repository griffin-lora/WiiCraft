#pragma once
#include <stddef.h>

typedef struct {
    size_t size;
    void* data;
} display_list_t;

typedef struct {
    size_t size;
    void* chunk;
    size_t chunk_index;
} pool_display_list_t;