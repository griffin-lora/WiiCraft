#pragma once
#include "block.h"
#include <gctypes.h>
#include <assert.h>

typedef struct {
    u8 px;
    u8 py;
    u8 pz;
    u8 txy;
} block_vertex_t;

typedef struct {
    block_vertex_t verts[4];
} block_quad_t;
_Static_assert(sizeof(block_quad_t) == 4 * 4, "");

#define NUM_SOLID_BUILDING_QUADS 0x100
#define NUM_TRANSPARENT_BUILDING_QUADS 0x100
#define NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_QUADS 0x100

typedef block_quad_t display_list_chunk_t[0x100];
_Static_assert(sizeof(display_list_chunk_t) == 4096, "");

typedef block_type_t block_chunk_t[16 * 16 * 16];
_Static_assert(sizeof(block_chunk_t) == 4096, "");

typedef struct {
    display_list_chunk_t secondary_display_list_chunks[3]; // These take up the same location in cache as the building quads and thus are the secondary choice in the event of running out of display list chunks
    display_list_chunk_t primary_display_list_chunks[4];
    block_chunk_t block_chunk;
} game_cache_chunk_t;

_Static_assert(sizeof(game_cache_chunk_t) == 32768, "");

typedef struct {
    // First cache chunk
    // Building quads (hopefully) locked in upper half of cache
    struct {
        _Alignas(32) block_quad_t solid[NUM_SOLID_BUILDING_QUADS];
        _Alignas(32) block_quad_t transparent[NUM_TRANSPARENT_BUILDING_QUADS];
        _Alignas(32) block_quad_t transparent_double_sided[NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_QUADS];
    } building_quads;
    _Alignas(4096) display_list_chunk_t display_list_chunks[4];
    _Alignas(4096) block_chunk_t block_chunk;
    // End of first cache chunk
    // The remaining cache chunks
    _Alignas(32768) game_cache_chunk_t cache_chunks[639];
} game_memory_t;
_Static_assert(sizeof(game_memory_t) == 0x1400000, "");

extern game_memory_t gmem;
_Static_assert(sizeof(gmem.building_quads) == 4096*3, "");