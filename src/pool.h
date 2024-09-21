#pragma once
#include "game_math.h"
#include "game/block.h"
#include <gctypes.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#define NULL_CHUNK_INDEX UINT16_MAX

#define NUM_BLOCK_DISPLAY_LIST_CHUNKS 512
#define NUM_BLOCK_DISPLAY_LIST_POOLS 3

typedef struct {
    u16 size;
    u16 chunk_index;
    f32 x;
    f32 y;
    f32 z;
} block_display_list_t;

typedef u8 block_display_list_chunk_t[4096];

#define NUM_ROW_BLOCKS_PER_BLOCK_CHUNK 16
#define NUM_BLOCKS_PER_BLOCK_CHUNK 4096

#define BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET (16 * 16)
#define BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET 16
#define BLOCKS_PER_BLOCK_CHUNK_X_OFFSET 1

#define NUM_XZ_ROW_BLOCK_CHUNKS 7
#define NUM_Y_ROW_BLOCK_CHUNKS 5
#define NUM_BLOCK_CHUNKS (NUM_XZ_ROW_BLOCK_CHUNKS * NUM_Y_ROW_BLOCK_CHUNKS * NUM_XZ_ROW_BLOCK_CHUNKS)

#define BLOCK_POOL_CHUNK_INDICES_Z_OFFSET (5 * 7)
#define BLOCK_POOL_CHUNK_INDICES_Y_OFFSET 7
#define BLOCK_POOL_CHUNK_INDICES_X_OFFSET 1

typedef struct {
    u8 pool_index;
    u16 chunk_index;
} block_display_list_chunk_descriptor_t;

typedef struct {
    block_display_list_chunk_descriptor_t disp_list_chunk_descriptors[16];
    alignas(32) block_type_t block_types[4096];
} block_chunk_t;

static_assert(sizeof(block_chunk_t) == (sizeof(block_type_t) * 4096) + 64, "");

#define BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT 0b00000001
#define BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED 0b00000010
#define BLOCK_CHUNK_FLAG_HAS_VALID_BLOCKS 0b00000100
#define BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS 0b00001000

extern u16 block_disp_list_pools_head[NUM_BLOCK_DISPLAY_LIST_POOLS];
extern block_display_list_t block_disp_list_pools_disp_lists[NUM_BLOCK_DISPLAY_LIST_POOLS][NUM_BLOCK_DISPLAY_LIST_CHUNKS];
extern block_display_list_chunk_t block_disp_list_pools_chunks[NUM_BLOCK_DISPLAY_LIST_POOLS][NUM_BLOCK_DISPLAY_LIST_CHUNKS];

extern u8 block_pool_chunk_indices[NUM_BLOCK_CHUNKS];
extern u8 block_pool_chunk_bitfields[NUM_BLOCK_CHUNKS];
extern block_chunk_t block_pool_chunks[NUM_BLOCK_CHUNKS];

void pool_init(void);

block_display_list_t* acquire_block_display_list_pool_chunk(size_t pool_index);
bool release_block_display_list_pool_chunk(size_t pool_index, u16 chunk_index);