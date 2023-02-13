#include "chunk_core.hpp"
#include "util.h"

world_location_wrap_t get_world_location_at_world_position(vec3_s32_t corner_pos, glm::vec3 pos) {
    math::vector3s32 chunk_pos = {
        (s32)floorf(pos.x / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.y / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        (s32)floorf(pos.z / NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    math::vector3s32 chunk_rel_pos = {
        chunk_pos.x - corner_pos.x,
        chunk_pos.y /*- corner_pos.y ok?? */,
        chunk_pos.z - corner_pos.z
    };

    if (chunk_rel_pos.x < 0 || chunk_rel_pos.x >= NUM_ROW_BLOCK_CHUNKS || chunk_rel_pos.y != 0 || chunk_rel_pos.z < 0 || chunk_rel_pos.z >= NUM_ROW_BLOCK_CHUNKS) {
        return world_location_wrap_t{ false };
    }

    math::vector3u8 block_rel_pos = {
        mod_s32(pos.x, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.y, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        mod_s32(pos.z, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    u8 chunk_index = block_pool_chunk_indices[(chunk_rel_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (chunk_rel_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET)];

    block_type_t* bl_tp = &block_pool_chunks[chunk_index].blocks[(block_rel_pos.z * 16 * 16) + (block_rel_pos.y * 16) + block_rel_pos.x];

    return world_location_wrap_t{
        .success = true,
        .val = {
            .ch_pos = chunk_pos,
            .bl_pos = block_rel_pos,
            .chunk_index = chunk_index,
            .bl_tp = bl_tp
        }
    };
}