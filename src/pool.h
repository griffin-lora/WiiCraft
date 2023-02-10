#pragma once
#include <gctypes.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

typedef u8 pool_chunk_t[4096];

#define NUM_BLOCK_DISPLAY_LIST_CHUNKS 512

typedef struct {
    u16 size;
    u16 chunk_index;
    f32 x;
    f32 y;
    f32 z;
} block_display_list_chunk_descriptor_t;

typedef struct {
    u16 head;
    block_display_list_chunk_descriptor_t descriptors[NUM_BLOCK_DISPLAY_LIST_CHUNKS];
    _Alignas(32) pool_chunk_t chunks[NUM_BLOCK_DISPLAY_LIST_CHUNKS];
} block_display_list_pool_t;

#define NUM_BLOCK_CHUNKS 2048

typedef struct {
    u16 chunk_index;
} block_chunk_descriptor_t;

typedef struct {
    u16 head;
    block_chunk_descriptor_t descriptors[NUM_BLOCK_CHUNKS];
    _Alignas(32) pool_chunk_t chunks[NUM_BLOCK_CHUNKS];
} block_pool_t;


extern block_display_list_pool_t solid_display_list_pool;
extern block_display_list_pool_t transparent_display_list_pool;
extern block_display_list_pool_t transparent_double_sided_display_list_pool;
extern block_pool_t block_pool;

typedef enum {
    block_display_list_type_solid,
    block_display_list_type_transparent,
    block_display_list_type_transparent_double_sided
} block_display_list_type_t;

void pool_init(void);

block_display_list_pool_t* get_block_display_list_pool(block_display_list_type_t type);
block_display_list_chunk_descriptor_t* acquire_block_display_list_pool_chunk(block_display_list_type_t type);
void release_block_display_list_pool_chunk(block_display_list_type_t type, u16 chunk_index);

block_chunk_descriptor_t acquire_block_pool_chunk(void);
void release_block_pool_chunk(u16 chunk_index);