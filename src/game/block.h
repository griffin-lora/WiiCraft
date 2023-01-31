#pragma once
#include <assert.h>

#pragma GCC push_options
#pragma GCC optimize("short-enums")

typedef enum {
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

typedef enum {
    block_face_front, // +x
    block_face_back, // -x
    block_face_top, // +y
    block_face_bottom, // -z
    block_face_right, // +z
    block_face_left // -z
} block_face_t;

#ifndef __INTELLISENSE__
_Static_assert(sizeof(block_type_t) == 1, "");
_Static_assert(sizeof(block_face_t) == 1, "");
#endif

#pragma GCC pop_options