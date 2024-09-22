#include "world_location.h"
#include "log.h"
#include "util.h"
#include <math.h>

// TODO: Refactor
world_location_wrap_t get_world_location_at_voxel_world_position(s32vec3s region_pos, vec3s voxel_world_pos) {
	vec3s div_pos = glms_vec3_scale(voxel_world_pos, 1.0f/(f32) NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
    s32vec3s chunk_pos = {
        .x = (s32) floorf(div_pos.x),
        .y = (s32) floorf(div_pos.y),
        .z = (s32) floorf(div_pos.z)
    };

    s32vec3s region_local_pos;
    glm_ivec3_sub(chunk_pos.raw, region_pos.raw, region_local_pos.raw);

    u8vec3s voxel_local_pos = {
        .x = (u8) mod_s32((s32) voxel_world_pos.x, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        .y = (u8) mod_s32((s32) voxel_world_pos.y, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK),
        .z = (u8) mod_s32((s32) voxel_world_pos.z, NUM_ROW_BLOCKS_PER_BLOCK_CHUNK)
    };

    size_t index = (size_t) ((region_local_pos.z * BLOCK_POOL_CHUNK_INDICES_Z_OFFSET) + (region_local_pos.y * BLOCK_POOL_CHUNK_INDICES_Y_OFFSET) + (region_local_pos.x * BLOCK_POOL_CHUNK_INDICES_X_OFFSET));

    if (index >= NUM_BLOCK_CHUNKS) {
        return (world_location_wrap_t){ .success = false };
    }

    size_t region_index = block_pool_chunk_indices[index];

    voxel_type_t* voxel_type = &block_pool_chunks[region_index].voxel_types[(voxel_local_pos.z * BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET) + (voxel_local_pos.y * BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET) + (voxel_local_pos.x * BLOCKS_PER_BLOCK_CHUNK_X_OFFSET)];

    return (world_location_wrap_t){
        .success = true,
        .val = {
            .region_pos = chunk_pos,
            .voxel_local_pos = voxel_local_pos,
            .voxel_type = voxel_type
        }
    };
}

vec3s get_world_position_from_world_location(const world_location_t* world_loc) {
    return (vec3s) {{
        (f32) (world_loc->region_pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK + world_loc->voxel_local_pos.x),
        (f32) (world_loc->region_pos.y * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK + world_loc->voxel_local_pos.y),
        (f32) (world_loc->region_pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK + world_loc->voxel_local_pos.z)
    }};
}