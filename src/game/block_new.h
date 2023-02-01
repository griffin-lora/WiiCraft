#pragma once
#include <assert.h>
#include <gctypes.h>
#include <assert.h>

typedef enum __attribute__((__packed__)) {
    block_type_air,
    block_type_debug,
    block_type_grass,
    block_type_stone,
    block_type_dirt,
    block_type_sand,
    block_type_wood_planks,
    block_type_stone_slab_bottom,
    block_type_stone_slab_top,
    block_type_stone_slab_both,
    block_type_tall_grass,
    block_type_water
} block_type_t;

typedef enum __attribute__((__packed__)) {
    block_face_front, // +x
    block_face_back, // -x
    block_face_top, // +y
    block_face_bottom, // -z
    block_face_right, // +z
    block_face_left // -z
} block_face_t;

_Static_assert(sizeof(block_type_t) == 1, "");
_Static_assert(sizeof(block_face_t) == 1, "");

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
    _Alignas(32) block_quad_t solid[NUM_SOLID_BUILDING_QUADS];
    _Alignas(32) block_quad_t transparent[NUM_TRANSPARENT_BUILDING_QUADS];
    _Alignas(32) block_quad_t transparent_double_sided[NUM_TRANSPARENT_DOUBLE_SIDED_BUILDING_QUADS];
} building_quads_t;

_Static_assert(sizeof(building_quads_t) == 4096*3, "");