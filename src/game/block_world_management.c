#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include <stdbool.h>
#include <string.h>

static bool tg = false;

#define NUM_TO_GENERATE (8 * 8)
#define Z_OFFSET 8
#define X_OFFSET 1

#define NUM_PER_ROW 8

void manage_block_world(void) {
    if (tg) {
        return;
    }
    tg = true;

    u16* chunk_indices = &block_pool.chunk_indices[block_pool.head];
    vec3_s32_t* positions = &block_pool.positions[block_pool.head];

    block_pool.head += NUM_PER_ROW * NUM_PER_ROW;

    size_t index = 0;

    for (s32 z = 0; z < NUM_PER_ROW; z++) {
        for (s32 x = 0; x < NUM_PER_ROW; x++) {
            vec3_s32_t pos = {
                .x = x,
                .y = 0,
                .z = z
            };

            positions[index] = pos;
            
            u16 chunk_index = chunk_indices[index];

            block_chunk_t* chunk = &block_pool.chunks[chunk_index];

            memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));
            
            chunk->front_chunk_index = (x == 15) ? NULL_CHUNK_INDEX : chunk_indices[index + X_OFFSET];
            chunk->back_chunk_index = (x == 0) ? NULL_CHUNK_INDEX : chunk_indices[index - X_OFFSET];
            chunk->top_chunk_index = NULL_CHUNK_INDEX;
            chunk->bottom_chunk_index = NULL_CHUNK_INDEX;
            chunk->right_chunk_index = (z == 15) ? NULL_CHUNK_INDEX : chunk_indices[index + Z_OFFSET];
            chunk->left_chunk_index = (z == 0) ? NULL_CHUNK_INDEX : chunk_indices[index - Z_OFFSET];

            generate_procedural_blocks(pos, chunk->blocks);

            index++;
        }
    }

    index = 0;

    for (s32 z = 0; z < NUM_PER_ROW; z++) {
        for (s32 x = 0; x < NUM_PER_ROW; x++) {
            vec3_s32_t pos = {
                .x = x,
                .y = 0,
                .z = z
            };

            u16 chunk_index = chunk_indices[index];
            block_chunk_t* chunk = &block_pool.chunks[chunk_index];

            update_block_chunk_visuals((vec3s){ pos.x * 16, 20, pos.z * 16 }, chunk);

            index++;
        }
    }
}