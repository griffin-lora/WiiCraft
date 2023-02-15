#include "chunk_core.h"
#include "util.h"
#include "block_world_management.h"
#include <math.h>

world_location_wrap_t get_world_location_at_world_position(s32vec3s corner_pos, vec3s pos) {
    s32vec3s chunk_pos = {{
        (s32)floorf(pos.x / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.y / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.z / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    }};

    s32vec3s chunk_rel_pos = {{
        chunk_pos.x - corner_pos.x,
        chunk_pos.y - corner_pos.y,
        chunk_pos.z - corner_pos.z
    }};

    u8vec3s block_rel_pos = {
        mod_s32(pos.x, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.y, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.z, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    size_t index = (chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET);

    if (index >= NUM_BLOCK_CHUNKS) {
        return (world_location_wrap_t){ false };
    }

    size_t chunk_index = block_pool_chunk_indices[index];

    block_type_t* bl_tp = &block_pool_chunks[chunk_index].block_types[(block_rel_pos.z * BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET) + (block_rel_pos.y * BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET) + (block_rel_pos.x * BLOCKS_PER_BLOCK_CHUNK_X_OFFSET)];

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

static void set_important_update(size_t index) {
    block_pool_chunk_bitfields[index] |= BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT;
    block_pool_chunk_bitfields[index] &= (~BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS);
}

bool update_block_chunk_and_neighbors(s32vec3s corner_pos, s32vec3s chunk_pos, u8vec3s modification_pos) {
    s32vec3s chunk_rel_pos = {{
        chunk_pos.x - corner_pos.x,
        chunk_pos.y - corner_pos.y,
        chunk_pos.z - corner_pos.z
    }};

    size_t index = (chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET);

    if (index >= NUM_BLOCK_CHUNKS) {
        return false;
    }

    u8* chunk_bitfields = block_pool_chunk_bitfields;

    if (chunk_pos.y != 0) {
        chunk_bitfields[index] &= (~BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS);
    }

    chunk_bitfields[index] |= BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT;

    if (modification_pos.x == NUM_ROW_BLOCKS_PER_BLOCK_CHUNK - 1 && chunk_rel_pos.x != NUM_XZ_ROW_BLOCK_CHUNKS - 1) {
        set_important_update(index + BLOCK_POOL_CHUNK_INDICES_X_OFFSET);
    }

    if (modification_pos.x == 0 && chunk_rel_pos.x != 0) {
        set_important_update(index - BLOCK_POOL_CHUNK_INDICES_X_OFFSET);
    }

    if (modification_pos.y == NUM_ROW_BLOCKS_PER_BLOCK_CHUNK - 1 && chunk_rel_pos.y != NUM_Y_ROW_BLOCK_CHUNKS - 1) {
        set_important_update(index + BLOCK_POOL_CHUNK_INDICES_Y_OFFSET);
    }

    if (modification_pos.y == 0 && chunk_rel_pos.y != 0) {
        set_important_update(index - BLOCK_POOL_CHUNK_INDICES_Y_OFFSET);
    }

    if (modification_pos.z == NUM_ROW_BLOCKS_PER_BLOCK_CHUNK - 1 && chunk_rel_pos.z != NUM_XZ_ROW_BLOCK_CHUNKS - 1) {
        set_important_update(index + BLOCK_POOL_CHUNK_INDICES_Z_OFFSET);
    }

    if (modification_pos.z == 0 && chunk_rel_pos.z != 0) {
        set_important_update(index - BLOCK_POOL_CHUNK_INDICES_Z_OFFSET);
    }

    return true;
}