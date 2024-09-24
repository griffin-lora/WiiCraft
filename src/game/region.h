#pragma once
#include "game/display_list.h"
#include "game/voxel.h"
#include "game_math.h"
#include <cglm/types-struct.h>
#include <gctypes.h>
#include <stddef.h>

#define REGION_SIZE 16

#define NUM_REGION_DISPLAY_LIST_ARRAYS 3

typedef struct {
    size_t num_display_lists;
    display_list_t* display_lists;
} region_display_list_array_t;

typedef struct {
    region_display_list_array_t display_list_arrays[NUM_REGION_DISPLAY_LIST_ARRAYS];
} region_render_info_t;

typedef struct {
    // types[x][y][z]
    voxel_type_t types[REGION_SIZE][REGION_SIZE][REGION_SIZE];
} voxel_type_array_t;

extern u32 world_size;
extern voxel_type_array_t** region_voxel_type_arrays;
extern region_render_info_t* region_render_infos;

inline size_t get_num_regions() {
    return world_size * world_size * world_size;
}

#define REGION_TYPE_3D(TYPE) typeof(TYPE (*)[world_size][world_size][world_size]) 
#define REGION_CAST_3D(TYPE, VAR) (TYPE (*)[world_size][world_size][world_size]) (VAR)

s32vec3s get_region_position_from_voxel_world_position(s32vec3s voxel_world_pos);