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

static size_t num_procedural_generate_queue_items = 0;
_Alignas(32) static block_chunk_update_t procedural_generate_queue[NUM_BLOCK_CHUNKS];

static size_t num_visuals_update_queue_items = 0;
_Alignas(32) static block_chunk_update_t visuals_update_queue[NUM_BLOCK_CHUNKS];

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
    block_chunk_t* chunks = block_pool.chunks;

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

            block_chunk_t* chunk = &chunks[chunk_index];

            memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

            chunk->front_chunk_index = NULL_CHUNK_INDEX;
            chunk->back_chunk_index = NULL_CHUNK_INDEX;
            chunk->top_chunk_index = NULL_CHUNK_INDEX;
            chunk->bottom_chunk_index = NULL_CHUNK_INDEX;
            chunk->right_chunk_index = NULL_CHUNK_INDEX;
            chunk->left_chunk_index = NULL_CHUNK_INDEX;
            
            procedural_generate_queue[num_procedural_generate_queue_items++] = (block_chunk_update_t){ pos, chunk_index };

            gen_index++;
        }
    }

    // Update neighborhoods of block chunks
    gen_index = 0;
    for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
        for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
            block_chunk_t* chunk = &chunks[local_chunk_indices[gen_index]];

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

    if (num_procedural_generate_queue_items > 0) {
        block_chunk_update_t update = procedural_generate_queue[--num_procedural_generate_queue_items];

        visuals_update_queue[num_visuals_update_queue_items++] = update;
        
        generate_procedural_blocks(update.position, chunks[update.chunk_index].blocks);
    } else if (num_visuals_update_queue_items > 0) { // Bit of a hacky fix, only start dealing with this queue once we don't need to do any procedural generation. This is to allow all procedural generation to finish before visuals updates.
        block_chunk_update_t* update = &visuals_update_queue[--num_visuals_update_queue_items];

        vec3s world_pos = { update->position.x * 16, 20, update->position.z * 16 };

        update_block_chunk_visuals(world_pos, &chunks[update->chunk_index]);
    }
}