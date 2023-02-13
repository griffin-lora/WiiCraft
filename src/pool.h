#pragma once
#include "math/vector.h"
#include "game/block.h"
#include <gctypes.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define NULL_CHUNK_INDEX UINT16_MAX

#define NUM_BLOCK_DISPLAY_LIST_CHUNKS 512

typedef enum __attribute__((__packed__)) {
    block_display_list_type_solid,
    block_display_list_type_transparent,
    block_display_list_type_transparent_double_sided
} block_display_list_type_t;

typedef struct {
    u16 size;
    u16 chunk_index;
    f32 x;
    f32 y;
    f32 z;
} block_display_list_t;

typedef u8 block_display_list_chunk_t[4096];

typedef struct {
    u16 head;
    block_display_list_t disp_lists[NUM_BLOCK_DISPLAY_LIST_CHUNKS];
    _Alignas(32) block_display_list_chunk_t chunks[NUM_BLOCK_DISPLAY_LIST_CHUNKS];
} block_display_list_pool_t;

#define NUM_ROW_BLOCKS_PER_BLOCK_CHUNK 16
#define NUM_BLOCKS_PER_BLOCK_CHUNK 4096

#define NUM_ROW_BLOCK_CHUNKS 8
#define NUM_BLOCK_CHUNKS (NUM_ROW_BLOCK_CHUNKS * NUM_ROW_BLOCK_CHUNKS)

typedef struct {
    block_display_list_type_t type;
    u16 chunk_index;
} block_display_list_chunk_descriptor_t;

typedef struct {
    block_display_list_chunk_descriptor_t disp_list_chunk_descriptors[16];
    _Alignas(32) block_type_t blocks[4096];
} block_chunk_t;

_Static_assert(sizeof(block_chunk_t) == (sizeof(block_type_t) * 4096) + 64, "");

typedef struct {
    u8 chunk_indices[NUM_BLOCK_CHUNKS];
    _Alignas(32) block_chunk_t chunks[NUM_BLOCK_CHUNKS];
} block_pool_t;

extern block_display_list_pool_t solid_display_list_pool;
extern block_display_list_pool_t transparent_display_list_pool;
extern block_display_list_pool_t transparent_double_sided_display_list_pool;
extern block_pool_t block_pool;

void pool_init(void);

block_display_list_pool_t* get_block_display_list_pool(block_display_list_type_t type);
block_display_list_t* acquire_block_display_list_pool_chunk(block_display_list_type_t type);
void release_block_display_list_pool_chunk(block_display_list_type_t type, u16 chunk_index);