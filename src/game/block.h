#pragma once
#include <sys/cdefs.h> // TODO: compiler stuff

typedef enum __attribute__((__packed__)) {
    block_type_air,
    block_type_debug,
    block_type_grass,
    block_type_stone,
    block_type_dirt,
    block_type_sand,
    block_type_wood_planks,
    block_type_stone_slab_both,
    block_type_water,
    block_type_tall_grass
} block_type_t;

typedef enum __attribute__((__packed__)) {
    block_face_front, // +x
    block_face_back, // -x
    block_face_top, // +y
    block_face_bottom, // -z
    block_face_right, // +z
    block_face_left // -z
} block_face_t;

static_assert(sizeof(block_type_t) == 1, "");
static_assert(sizeof(block_face_t) == 1, "");