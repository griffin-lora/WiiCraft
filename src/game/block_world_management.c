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
    u16 chunk_index;
    vec3_s32_t position;
} block_chunk_update_t;

static size_t num_procedural_generate_queue_items = 0;
_Alignas(32) static block_chunk_update_t procedural_generate_queue[NUM_BLOCK_CHUNKS];

static size_t num_visuals_update_queue_items = 0;
_Alignas(32) static block_chunk_update_t visuals_update_queue[NUM_BLOCK_CHUNKS];

_Alignas(32) static u16 local_chunk_indices[NUM_TO_GENERATE];

_Alignas(32) static u16 chunk_indices_to_release[NUM_TO_GENERATE];

static void remove_chunks_outside_of_range_from_queue(vec3_s32_t center_pos, size_t num_items, block_chunk_update_t queue[]) {
    for (size_t i = 0; i < num_items; i++) {
        block_chunk_update_t* update = &queue[i];

        vec3_s32_t pos = update->position;
        vec3_s32_t rel_pos = { pos.x - center_pos.x, 0, pos.z - center_pos.z };
        if (rel_pos.x >= 0 && rel_pos.x < NUM_PER_GENERATION_ROW && rel_pos.z >= 0 && rel_pos.z < NUM_PER_GENERATION_ROW) {
            continue;
        }

        update->chunk_index = NULL_CHUNK_INDEX;
    }
}

static void fill_local_chunk_indices(vec3_s32_t center_pos) {
    size_t num_chunk_indices_to_release = 0;

    memset(local_chunk_indices, 0xff, sizeof(local_chunk_indices));

    u16* chunk_indices = block_pool.chunk_indices;
    vec3_s32_t* positions = block_pool.positions;
    
    for (size_t i = 0; i < block_pool.head; i++) {
        vec3_s32_t pos = positions[i];
        vec3_s32_t rel_pos = { pos.x - center_pos.x, 0, pos.z - center_pos.z };
        if (rel_pos.x >= 0 && rel_pos.x < NUM_PER_GENERATION_ROW && rel_pos.z >= 0 && rel_pos.z < NUM_PER_GENERATION_ROW) {
            local_chunk_indices[(rel_pos.z * Z_OFFSET) + (rel_pos.x * X_OFFSET)] = chunk_indices[i];
        } else {
            chunk_indices_to_release[num_chunk_indices_to_release++] = chunk_indices[i];
        }
    }

    block_chunk_t* chunks = block_pool.chunks;

    for (size_t i = 0; i < num_chunk_indices_to_release; i++) {
        u16 chunk_index = chunk_indices_to_release[i];

        block_chunk_t* chunk = &chunks[chunk_index];

        block_display_list_chunk_descriptor_t* descriptors = chunk->disp_list_chunk_descriptors;
        for (size_t i = 0; descriptors[i].type != 0xff; i++) {
            release_block_display_list_pool_chunk(descriptors[i].type, descriptors[i].chunk_index);
        }
        
        if (chunk->front_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->front_chunk_index].back_chunk_index = NULL_CHUNK_INDEX; }
        if (chunk->back_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->back_chunk_index].front_chunk_index = NULL_CHUNK_INDEX; }
        if (chunk->top_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->top_chunk_index].bottom_chunk_index = NULL_CHUNK_INDEX; }
        if (chunk->bottom_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->bottom_chunk_index].top_chunk_index = NULL_CHUNK_INDEX; }
        if (chunk->right_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->right_chunk_index].left_chunk_index = NULL_CHUNK_INDEX; }
        if (chunk->left_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->left_chunk_index].right_chunk_index = NULL_CHUNK_INDEX; }

        release_block_pool_chunk(chunk_index);
    }

    remove_chunks_outside_of_range_from_queue(center_pos, num_procedural_generate_queue_items, procedural_generate_queue);
    remove_chunks_outside_of_range_from_queue(center_pos, num_visuals_update_queue_items, visuals_update_queue);
}

void manage_block_world(vec3_s32_t center_pos) {
    fill_local_chunk_indices(center_pos);

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
                .x = center_pos.x + x,
                .y = 0,
                .z = center_pos.z + z
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
            
            procedural_generate_queue[num_procedural_generate_queue_items++] = (block_chunk_update_t){ chunk_index, pos };

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
        while (num_procedural_generate_queue_items > 0) {
            block_chunk_update_t update = procedural_generate_queue[--num_procedural_generate_queue_items];

            if (update.chunk_index == NULL_CHUNK_INDEX) {
                continue;
            }

            visuals_update_queue[num_visuals_update_queue_items++] = update;
            
            generate_procedural_blocks(update.position, chunks[update.chunk_index].blocks);
            break;
        }
    } else if (num_visuals_update_queue_items > 0) { // Bit of a hacky fix, only start dealing with this queue once we don't need to do any procedural generation. This is to allow all procedural generation to finish before visuals updates.
        while (num_visuals_update_queue_items > 0) {
            block_chunk_update_t* update = &visuals_update_queue[--num_visuals_update_queue_items];

            if (update->chunk_index == NULL_CHUNK_INDEX) {
                continue;
            }

            vec3s world_pos = { update->position.x * 16, 0, update->position.z * 16 };

            update_block_chunk_visuals(world_pos, &chunks[update->chunk_index]);
            break;
        }
    }
}