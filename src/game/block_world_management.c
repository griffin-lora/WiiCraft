#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include <stdbool.h>
#include <string.h>

static bool x = false;

void manage_block_world(void) {
    if (x) {
        return;
    }
    x = true;

    vec3_s32_t pos = {
        .x = 0,
        .y = 0,
        .z = 0
    };

    u16 index = acquire_block_pool_chunk();
    block_pool.positions[index] = pos;
    
    u16 chunk_index = block_pool.chunk_indices[index];
    block_chunk_t* chunk = &block_pool.chunks[chunk_index];
    memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));
    chunk->front_chunk_index = NULL_CHUNK_INDEX;
    chunk->back_chunk_index = NULL_CHUNK_INDEX;
    chunk->top_chunk_index = NULL_CHUNK_INDEX;
    chunk->bottom_chunk_index = NULL_CHUNK_INDEX;
    chunk->right_chunk_index = NULL_CHUNK_INDEX;
    chunk->left_chunk_index = NULL_CHUNK_INDEX;

    generate_procedural_blocks(pos, chunk->blocks);

    update_block_chunk_visuals((vec3s){ 0, 16, 0 }, chunk);
}