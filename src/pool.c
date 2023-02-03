#include "pool.h"
#include <string.h>

_Alignas(32) pool_chunks_info_t pool_chunks_info = {
    .head = 0
};

// Aligned to beginning of L2 cache, along with L1 cache
_Alignas(0x40000) pool_chunk_t pool_chunks[NUM_POOL_CHUNKS];

void pool_init(void) {
    memset(pool_chunks_info.used, 0, sizeof(pool_chunks_info.used));
}

size_t acquire_pool_chunk(void) {
    size_t head = pool_chunks_info.head;
    for (;;) {
        u32 bit_index = head % 8;
        if (!((pool_chunks_info.used[head / 8] >> bit_index) & 0x1)) {
            pool_chunks_info.used[head / 8] |= 0x1 << bit_index;
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
    pool_chunks_info.used[index / 8] ^= 0x1 << (index % 8);
    pool_chunks_info.head = index;
}