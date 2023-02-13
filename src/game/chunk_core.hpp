#pragma once
#include "chunk.hpp"
#include "chunk_math.hpp"
#include "chrono.hpp"
#include "pool.hpp"

typedef struct {
    math::vector3s32 ch_pos;
    math::vector3s32 bl_pos;
    u8 chunk_index;
    block_type_t* bl_tp;
} world_location_t;

typedef struct {
    bool success;
    world_location_t val;
} world_location_wrap_t;

world_location_wrap_t get_world_location_at_world_position(vec3_s32_t corner_pos, glm::vec3 pos);