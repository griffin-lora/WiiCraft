#pragma once
#include <gctypes.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define NUM_POOL_CHUNKS 4864

typedef struct {
    u16 head;
    // Every index before the head is used, every index after the head is unused
    u16 chunk_indices[NUM_POOL_CHUNKS];
} pool_chunks_info_t;

extern pool_chunks_info_t pool_chunks_info;

typedef u8 pool_chunk_t[4096];

extern pool_chunk_t pool_chunks[NUM_POOL_CHUNKS];
_Static_assert(sizeof(pool_chunks) == 0x1300000, "");

#define NULL_POOL_CHUNK_INDEX UINT16_MAX

void pool_init(void);

u16 acquire_pool_chunk(void);
void release_pool_chunk(u16 chunk_index);