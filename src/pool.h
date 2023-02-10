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

#define NUM_BLOCK_CHUNKS 2048

typedef struct {
    block_display_list_type_t type;
    u16 chunk_index;
} block_display_list_chunk_descriptor_t;

typedef struct {
    block_display_list_chunk_descriptor_t disp_list_chunk_descriptors[16];

    _Alignas(32) u16 front_chunk_index;
    u16 back_chunk_index;
    u16 top_chunk_index;
    u16 bottom_chunk_index;
    u16 right_chunk_index;
    u16 left_chunk_index;
    
    _Alignas(32) block_type_t blocks[4096];
} block_chunk_t;

_Static_assert(sizeof(block_chunk_t) == (sizeof(block_type_t) * 4096) + 96, "");

typedef struct {
    u16 head;
    u16 chunk_indices[NUM_BLOCK_CHUNKS];
    _Alignas(32) vec3_s32_t positions[NUM_BLOCK_CHUNKS];
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

u16 acquire_block_pool_chunk(void);
void release_block_pool_chunk(u16 chunk_index);