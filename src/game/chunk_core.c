#include "chunk_core.h"
#include "util.h"
#include "block_world_management.h"
#include <math.h>

world_location_wrap_t get_world_location_at_world_position(vec3_s32_t corner_pos, vec3s pos) {
    vec3_s32_t chunk_pos = {
        (s32)floorf(pos.x / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.y / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.z / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    vec3_s32_t chunk_rel_pos = {
        chunk_pos.x - corner_pos.x,
        chunk_pos.y - corner_pos.y,
        chunk_pos.z - corner_pos.z
    };

    vec3_u8_t block_rel_pos = {
        mod_s32(pos.x, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.y, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.z, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    size_t index = (chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET);

    if (index >= NUM_BLOCK_CHUNKS) {
        return (world_location_wrap_t){ false };
    }

    size_t chunk_index = block_pool_chunk_indices[index];

    block_type_t* bl_tp = &block_pool_chunks[chunk_index].blocks[(block_rel_pos.z * BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET) + (block_rel_pos.y * BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET) + (block_rel_pos.x * BLOCKS_PER_BLOCK_CHUNK_X_OFFSET)];

    return (world_location_wrap_t){
        .success = true,
        .val = {
            .ch_pos = chunk_pos,
            .bl_pos = block_rel_pos,
            .chunk_index = chunk_index,
            .bl_tp = bl_tp
        }
    };
}

void update_block_chunk_and_neighbors(vec3_s32_t corner_pos, vec3_s32_t chunk_pos) {
    vec3_s32_t chunk_rel_pos = {
        chunk_pos.x - corner_pos.x,
        chunk_pos.y - corner_pos.y,
        chunk_pos.z - corner_pos.z
    };

    size_t index = (chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET);

    if (index >= NUM_BLOCK_CHUNKS) {
        return;
    }

    u8* chunk_indices = block_pool_chunk_indices;
    block_chunk_t* chunks = block_pool_chunks;

    if (chunk_pos.y != 0) {
        chunks[chunk_indices[index]].has_trivial_visuals = false;

        if (chunk_rel_pos.x != NUM_XZ_ROW_BLOCK_CHUNKS - 1) { chunks[chunk_indices[index + BLOCK_POOL_CHUNK_INDICES_X_OFFSET]].has_trivial_visuals = false; }
        if (chunk_rel_pos.x != 0) { chunks[chunk_indices[index - BLOCK_POOL_CHUNK_INDICES_X_OFFSET]].has_trivial_visuals = false; }
        if (chunk_rel_pos.z != NUM_XZ_ROW_BLOCK_CHUNKS - 1) { chunks[chunk_indices[index + BLOCK_POOL_CHUNK_INDICES_Z_OFFSET]].has_trivial_visuals = false; }
        if (chunk_rel_pos.z != 0) { chunks[chunk_indices[index - BLOCK_POOL_CHUNK_INDICES_Z_OFFSET]].has_trivial_visuals = false; }
    }

    if (chunk_rel_pos.y != NUM_Y_ROW_BLOCK_CHUNKS - 1) { chunks[chunk_indices[index + BLOCK_POOL_CHUNK_INDICES_Y_OFFSET]].has_trivial_visuals = false; }
    if (chunk_rel_pos.y != 0) { chunks[chunk_indices[index - BLOCK_POOL_CHUNK_INDICES_Y_OFFSET]].has_trivial_visuals = false; }

    visuals_update_queue[num_visuals_update_queue_items++] = chunk_pos;
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x + 1, chunk_pos.y, chunk_pos.z };
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x - 1, chunk_pos.y, chunk_pos.z };
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x, chunk_pos.y + 1, chunk_pos.z };
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x, chunk_pos.y - 1, chunk_pos.z };
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x, chunk_pos.y, chunk_pos.z + 1 };
    visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){ chunk_pos.x, chunk_pos.y, chunk_pos.z - 1 };
}