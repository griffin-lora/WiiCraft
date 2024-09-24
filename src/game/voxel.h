#pragma once
#include "game_math.h"
#include <sys/cdefs.h> // TODO: compiler stuff

typedef enum __attribute__((__packed__)) {
    voxel_type_air,
    voxel_type_debug,
    voxel_type_grass,
    voxel_type_stone,
    voxel_type_dirt,
    voxel_type_sand,
    voxel_type_wood_planks,
    voxel_type_stone_slab_both,
    voxel_type_water,
    voxel_type_tall_grass
} voxel_type_t;

typedef enum __attribute__((__packed__)) {
    voxel_face_front, // +x
    voxel_face_back, // -x
    voxel_face_top, // +y
    voxel_face_bottom, // -z
    voxel_face_right, // +z
    voxel_face_left // -z
} voxel_face_t;

static_assert(sizeof(voxel_type_t) == 1, "");
static_assert(sizeof(voxel_face_t) == 1, "");

s32vec3s get_voxel_world_position(vec3s world_pos);
u32vec3s get_voxel_local_position_from_voxel_world_position(s32vec3s voxel_world_pos);