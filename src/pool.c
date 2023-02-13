#include "pool.h"
#include "log.h"
#include <string.h>

_Alignas(32) block_display_list_pool_t solid_display_list_pool = {
    .head = 0
};
_Alignas(32) block_display_list_pool_t transparent_display_list_pool = {
    .head = 0
};
_Alignas(32) block_display_list_pool_t transparent_double_sided_display_list_pool = {
    .head = 0
};

_Alignas(32) u8 block_pool_chunk_indices[NUM_BLOCK_CHUNKS];
_Alignas(32) u8 block_pool_chunk_bitfields[NUM_BLOCK_CHUNKS];
_Alignas(32) block_chunk_t block_pool_chunks[NUM_BLOCK_CHUNKS];

static void init_display_list_pool(block_display_list_pool_t* pool) {
    block_display_list_t* disp_lists = pool->disp_lists;
    for (size_t i = 0; i < NUM_BLOCK_DISPLAY_LIST_CHUNKS; i++) {
        disp_lists[i].chunk_index = (u16)i;
    }
}

void pool_init(void) {
    init_display_list_pool(&solid_display_list_pool);
    init_display_list_pool(&transparent_display_list_pool);
    init_display_list_pool(&transparent_double_sided_display_list_pool);
    u8* chunk_indices = block_pool_chunk_indices;
    for (size_t i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        chunk_indices[i] = (u8)i;
    }
    memset(block_pool_chunk_bitfields, 0, sizeof(block_pool_chunk_bitfields));
}

block_display_list_pool_t* get_block_display_list_pool(block_display_list_type_t type) {
    switch (type) {
        case block_display_list_type_solid: return &solid_display_list_pool;
        case block_display_list_type_transparent: return &transparent_display_list_pool;
        case block_display_list_type_transparent_double_sided: return &transparent_double_sided_display_list_pool;
    }
    return NULL;
}

block_display_list_t* acquire_block_display_list_pool_chunk(block_display_list_type_t type) {
    block_display_list_pool_t* pool = get_block_display_list_pool(type);
    return &pool->disp_lists[pool->head++];
}

bool release_block_display_list_pool_chunk(block_display_list_type_t type, u16 chunk_index) {
    block_display_list_pool_t* pool = get_block_display_list_pool(type);
    
    // This is really slow unfortunately
    // 87.5% of cacheline is wasted when erasing, this needs work

    u16 head = pool->head;
    block_display_list_t* disp_lists = pool->disp_lists;
    
    for (size_t i = 0; i < head; i++) {
        if (disp_lists[i].chunk_index == chunk_index) {
            memmove(&disp_lists[i], &disp_lists[i + 1], (head - i - 1) * sizeof(block_display_list_t));
            head--;
            disp_lists[head].chunk_index = chunk_index;

            pool->head = head;
            
            return true;
        }
    }
    // We should never reach here, report an error if we do
    lprintf("block_display_list_pool on chunk_index: %d double release occurred.\n", chunk_index);
    return false;
}