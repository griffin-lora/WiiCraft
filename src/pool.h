#pragma once
#include <gctypes.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define NUM_POOL_CHUNKS 4864

typedef struct {
    u16 head;
    u8 used[(NUM_POOL_CHUNKS / 8) + 1];
} pool_chunks_info_t;

extern pool_chunks_info_t pool_chunks_info;

typedef u8 pool_chunk_t[4096];

extern pool_chunk_t pool_chunks[NUM_POOL_CHUNKS];
_Static_assert(sizeof(pool_chunks) == 0x1300000, "");

#define NULL_POOL_CHUNK_INDEX UINT32_MAX

void pool_init(void);

size_t acquire_pool_chunk(void);
void release_pool_chunk(size_t index);