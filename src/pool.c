#include "pool.h"
#include <string.h>

_Alignas(32) pool_chunks_info_t pool_chunks_info = {
    .head = 0
};

// Aligned to beginning of L2 cache, along with L1 cache
_Alignas(0x40000) pool_chunk_t pool_chunks[NUM_POOL_CHUNKS];

void pool_init(void) {
    u16* chunk_indices = pool_chunks_info.chunk_indices;
    for (u16 i = 0; i < NUM_POOL_CHUNKS; i++) {
        chunk_indices[i] = i;
    }
}

u16 acquire_pool_chunk(void) {
    return pool_chunks_info.chunk_indices[pool_chunks_info.head++];
}

void release_pool_chunk(u16 chunk_index) {
    // This is pretty slow unfortunately
    u16 head = pool_chunks_info.head;
    u16* chunk_indices = pool_chunks_info.chunk_indices;
    
    for (u16 i = 0; i < NUM_POOL_CHUNKS; i++) {
        if (chunk_indices[i] == chunk_index) {
            memmove(&chunk_indices[i], &chunk_indices[i + 1], (head - i - 1) * sizeof(u16));
            head--;
            chunk_indices[head] = chunk_index;
            
            break;
        }
    }

    pool_chunks_info.head = head;
}