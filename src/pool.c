#include "pool.h"
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
_Alignas(32) block_pool_t block_pool = {
    .head = 0
};

static void init_display_list_pool(block_display_list_pool_t* pool) {
    block_display_list_chunk_descriptor_t* descriptors = pool->descriptors;
    for (u16 i = 0; i < NUM_BLOCK_DISPLAY_LIST_CHUNKS; i++) {
        descriptors[i].chunk_index = i;
    }
}

void pool_init(void) {
    init_display_list_pool(&solid_display_list_pool);
    init_display_list_pool(&transparent_display_list_pool);
    init_display_list_pool(&transparent_double_sided_display_list_pool);
    block_chunk_descriptor_t* descriptors = block_pool.descriptors;
    for (u16 i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        descriptors[i].chunk_index = i;
    }
}

block_display_list_pool_t* get_block_display_list_pool(block_display_list_type_t type) {
    switch (type) {
        case block_display_list_type_solid: return &solid_display_list_pool;
        case block_display_list_type_transparent: return &transparent_display_list_pool;
        case block_display_list_type_transparent_double_sided: return &transparent_double_sided_display_list_pool;
    }
}

block_display_list_chunk_descriptor_t* acquire_block_display_list_pool_chunk(block_display_list_type_t type) {
    block_display_list_pool_t* pool = get_block_display_list_pool(type);
    return &pool->descriptors[pool->head++];
}

void release_block_display_list_pool_chunk(block_display_list_type_t type, u16 chunk_index) {
    block_display_list_pool_t* pool = get_block_display_list_pool(type);
    
    // This is really slow unfortunately
    // 87.5% of cacheline is wasted when erasing, this needs work

    u16 head = pool->head;
    block_display_list_chunk_descriptor_t* descriptors = pool->descriptors;
    
    for (u16 i = 0; i < NUM_BLOCK_DISPLAY_LIST_CHUNKS; i++) {
        if (descriptors[i].chunk_index == chunk_index) {
            memmove(&descriptors[i], &descriptors[i + 1], (head - i - 1) * sizeof(block_display_list_chunk_descriptor_t));
            head--;
            descriptors[head].chunk_index = chunk_index;
            
            break;
        }
    }

    pool->head = head;
}

block_chunk_descriptor_t acquire_block_pool_chunk(void) {
    return block_pool.descriptors[block_pool.head++];
}

void release_block_pool_chunk(u16 chunk_index) {
    // This is pretty slow unfortunately
    u16 head = block_pool.head;
    block_chunk_descriptor_t* descriptors = block_pool.descriptors;
    
    for (u16 i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        if (descriptors[i].chunk_index == chunk_index) {
            memmove(&descriptors[i], &descriptors[i + 1], (head - i - 1) * sizeof(block_chunk_descriptor_t));
            head--;
            descriptors[head].chunk_index = chunk_index;
            
            break;
        }
    }

    block_pool.head = head;
}