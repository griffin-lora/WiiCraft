#pragma once
#include "game/block.h"
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
    // Building quads in upper half of cache
    struct {
        _Alignas(32) block_quad_t solid[NUM_SOLID_BUILDING_QUADS];
        _Alignas(32) block_quad_t transparent[NUM_TRANSPARENT_BUILDING_QUADS];
        _Alignas(32) block_quad_t transparent_double_sided[NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_QUADS];
    } building_quads;
    // display_list_chunk_t display_list_chunks_a[4];
    // block_chunk_t block_chunk_a;
} game_memory_t;

extern game_memory_t gmem;
_Static_assert(sizeof(gmem.building_quads) == 4096*3, "");