#include "pool.h"
#include <string.h>

_Alignas(32) pool_chunks_info_t pool_chunks_info = {
    .head = 0
};

// Aligned to beginning of L2 cache, along with L1 cache
_Alignas(0x40000) pool_chunk_t pool_chunks[NUM_POOL_CHUNKS];

void pool_init(void) {
    memset(pool_chunks_info.free, 1, sizeof(pool_chunks_info.free));
}

size_t acquire_pool_chunk(void) {
    size_t head = pool_chunks_info.head;
    for (;;) {
        if (pool_chunks_info.free[head]) {
            pool_chunks_info.free[head] = false;
            pool_chunks_info.head = head + 1;
            pool_chunks_info.head %= NUM_POOL_CHUNKS;

            return head;
        }

        head++;
        head %= NUM_POOL_CHUNKS;

        if (head == pool_chunks_info.head) {
            return NULL_POOL_CHUNK_INDEX;
        }
    }
}

void release_pool_chunk(size_t index) {
    pool_chunks_info.free[index] = true;
    pool_chunks_info.head = index;
}