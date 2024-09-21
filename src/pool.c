#include "pool.h"
#include "log.h"
#include <string.h>

_Alignas(32) u16 block_disp_list_pools_head[NUM_BLOCK_DISPLAY_LIST_POOLS];
_Alignas(32) block_display_list_t block_disp_list_pools_disp_lists[NUM_BLOCK_DISPLAY_LIST_POOLS][NUM_BLOCK_DISPLAY_LIST_CHUNKS];
_Alignas(32) block_display_list_chunk_t block_disp_list_pools_chunks[NUM_BLOCK_DISPLAY_LIST_POOLS][NUM_BLOCK_DISPLAY_LIST_CHUNKS];

_Alignas(32) u8 block_pool_chunk_indices[NUM_BLOCK_CHUNKS];
_Alignas(32) u8 block_pool_chunk_bitfields[NUM_BLOCK_CHUNKS];
_Alignas(32) block_chunk_t block_pool_chunks[NUM_BLOCK_CHUNKS];

void pool_init(void) {
    for (size_t i = 0; i < NUM_BLOCK_DISPLAY_LIST_POOLS; i++) {
        block_disp_list_pools_head[i] = 0;

        block_display_list_t* disp_lists = block_disp_list_pools_disp_lists[i];
        for (size_t j = 0; j < NUM_BLOCK_DISPLAY_LIST_CHUNKS; j++) {
            disp_lists[j].chunk_index = (u16)j;
        }
    }
    u8* chunk_indices = block_pool_chunk_indices;
    for (size_t i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        chunk_indices[i] = (u8)i;
    }
    memset(block_pool_chunk_bitfields, 0, sizeof(block_pool_chunk_bitfields));
}

block_display_list_t* acquire_block_display_list_pool_chunk(size_t pool_index) {
    if (block_disp_list_pools_head[pool_index] >= NUM_BLOCK_DISPLAY_LIST_CHUNKS) {
        lprintf("Block display list pool %ld, has hit allocation limit\n", pool_index);
        return NULL;
    }
    return &(block_disp_list_pools_disp_lists[pool_index])[block_disp_list_pools_head[pool_index]++];
}

bool release_block_display_list_pool_chunk(size_t pool_index, u16 chunk_index) {
    // This is really slow unfortunately
    // 87.5% of cacheline is wasted when erasing, this needs work

    u16 head = block_disp_list_pools_head[pool_index];
    block_display_list_t* disp_lists = block_disp_list_pools_disp_lists[pool_index];
    
    for (size_t i = 0; i < head; i++) {
        if (disp_lists[i].chunk_index == chunk_index) {
            memmove(&disp_lists[i], &disp_lists[i + 1], (head - i - 1) * sizeof(block_display_list_t));
            head--;
            disp_lists[head].chunk_index = chunk_index;

            block_disp_list_pools_head[pool_index] = head;
            
            return true;
        }
    }
    // We should never reach here, report an error if we do
    lprintf("Block display list pool %ld on chunk_index: %d double release occurred.\n", pool_index, chunk_index);
    return false;
}