#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include "log.h"
#include <stdbool.h>
#include <string.h>

#define Z_OFFSET 8
#define X_OFFSET 1

#define NUM_PER_GENERATION_ROW 8
#define NUM_TO_GENERATE (NUM_PER_GENERATION_ROW * NUM_PER_GENERATION_ROW)

typedef struct {
    vec3_s32_t position;
    u16 chunk_index;
} block_chunk_update_t;

_Alignas(32) static block_chunk_update_t procedural_generate_queue[NUM_BLOCK_CHUNKS];
block_chunk_update_t* procedural_generate_queue_begin = procedural_generate_queue;
block_chunk_update_t* procedural_generate_queue_end = procedural_generate_queue;

_Alignas(32) static block_chunk_update_t mesh_generate_queue[NUM_BLOCK_CHUNKS];
block_chunk_update_t* mesh_generate_queue_begin = mesh_generate_queue;
block_chunk_update_t* mesh_generate_queue_end = mesh_generate_queue;

_Alignas(32) static u16 local_chunk_indices[NUM_TO_GENERATE];

static void fill_local_chunk_indices(void) {
    memset(local_chunk_indices, 0xff, sizeof(local_chunk_indices));

    u16* chunk_indices = block_pool.chunk_indices;
    vec3_s32_t* positions = block_pool.positions;
    
    for (size_t i = 0; i < block_pool.head; i++) {
        vec3_s32_t pos = positions[i];
        // TODO: Make relative to management position
        if (pos.x >= 0 && pos.x < NUM_PER_GENERATION_ROW && pos.z >= 0 && pos.z < NUM_PER_GENERATION_ROW) {
            local_chunk_indices[(pos.z * Z_OFFSET) + (pos.x * X_OFFSET)] = chunk_indices[i];
        }
    }
}

void manage_block_world(void) {
    fill_local_chunk_indices();

    u16* chunk_indices = block_pool.chunk_indices;
    vec3_s32_t* positions = block_pool.positions;

    size_t gen_index = 0;

    // Allocate new block chunks
    for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
        for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
            if (local_chunk_indices[gen_index] != NULL_CHUNK_INDEX) {
                gen_index++;
                continue;
            }

            vec3_s32_t pos = {
                .x = x,
                .y = 0,
                .z = z
            };

            u16 index = acquire_block_pool_chunk();

            positions[index] = pos;
            u16 chunk_index = chunk_indices[index];

            local_chunk_indices[gen_index] = chunk_index;

            block_chunk_t* chunk = &block_pool.chunks[chunk_index];

            memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));
            
            *procedural_generate_queue_end = (block_chunk_update_t){ pos, chunk_index };
            procedural_generate_queue_end++;

            gen_index++;
        }
    }

    // Update neighborhoods of block chunks
    gen_index = 0;
    for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
        for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
            block_chunk_t* chunk = &block_pool.chunks[local_chunk_indices[gen_index]];

            chunk->top_chunk_index = NULL_CHUNK_INDEX;
            chunk->bottom_chunk_index = NULL_CHUNK_INDEX;

            if (x != (NUM_PER_GENERATION_ROW - 1) && chunk->front_chunk_index == NULL_CHUNK_INDEX) {
                chunk->front_chunk_index = local_chunk_indices[gen_index + X_OFFSET];
            }
            if (x != 0 && chunk->back_chunk_index == NULL_CHUNK_INDEX) {
                chunk->back_chunk_index = local_chunk_indices[gen_index - X_OFFSET];
            }
            if (z != (NUM_PER_GENERATION_ROW - 1) && chunk->right_chunk_index == NULL_CHUNK_INDEX) {
                chunk->right_chunk_index = local_chunk_indices[gen_index + Z_OFFSET];
            }
            if (z != 0 && chunk->left_chunk_index == NULL_CHUNK_INDEX) {
                chunk->left_chunk_index = local_chunk_indices[gen_index - Z_OFFSET];
            }

            gen_index++;
        }
    }
}