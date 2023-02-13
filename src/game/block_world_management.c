#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include "log.h"
#include <stdbool.h>
#include <string.h>

#define Z_OFFSET 8
#define X_OFFSET 1

size_t num_procedural_generate_queue_items = 0;
_Alignas(32) vec3_s32_t procedural_generate_queue[NUM_WORLD_QUEUE_ITEMS];

size_t num_visuals_update_queue_items = 0;
_Alignas(32) vec3_s32_t visuals_update_queue[NUM_WORLD_QUEUE_ITEMS];

static void remove_chunks_outside_of_range_from_queue(vec3_s32_t corner_pos, size_t num_items, vec3_s32_t queue[]) {
    for (size_t i = 0; i < num_items; i++) {
        vec3_s32_t pos = queue[i];
        vec3_s32_t rel_pos = { pos.x - corner_pos.x, 0, pos.z - corner_pos.z };
        if (rel_pos.x >= 0 && rel_pos.x < NUM_ROW_BLOCK_CHUNKS && rel_pos.z >= 0 && rel_pos.z < NUM_ROW_BLOCK_CHUNKS) {
            continue;
        }

        queue[i].x = UINT32_MAX;
    }
}

void init_block_world(vec3_s32_t corner_pos) {
    for (s32 z = 0; z < NUM_ROW_BLOCK_CHUNKS; z++) {
        for (s32 x = 0; x < NUM_ROW_BLOCK_CHUNKS; x++) {
            procedural_generate_queue[num_procedural_generate_queue_items++] = (vec3_s32_t){
                .x = x + corner_pos.x,
                .y = 0,
                .z = z + corner_pos.z
            };
            visuals_update_queue[num_visuals_update_queue_items++] = (vec3_s32_t){
                .x = x + corner_pos.x,
                .y = 0,
                .z = z + corner_pos.z
            };
        }
    }
}

static void fill_local_chunk_indices(vec3_s32_t corner_pos) {
    // u16* chunk_indices = block_pool.chunk_indices;
    // vec3_s32_t* positions = block_pool.positions;
    // block_chunk_t* chunks = block_pool.chunks;
    
    // for (size_t i = 0; i < block_pool.head; i++) {
    //     u16 chunk_index = chunk_indices[i];
    //     vec3_s32_t pos = positions[i];
    //     vec3_s32_t rel_pos = { pos.x - corner_pos.x, 0, pos.z - corner_pos.z };

    //     if (rel_pos.x >= 0 && rel_pos.x < NUM_PER_GENERATION_ROW && rel_pos.z >= 0 && rel_pos.z < NUM_PER_GENERATION_ROW) {
    //         local_chunk_indices[(rel_pos.z * Z_OFFSET) + (rel_pos.x * X_OFFSET)] = chunk_index;
    //     } else {
    //         block_chunk_t* chunk = &chunks[chunk_index];

    //         block_display_list_chunk_descriptor_t* descriptors = chunk->disp_list_chunk_descriptors;
    //         for (size_t i = 0; descriptors[i].type != 0xff; i++) {
    //             release_block_display_list_pool_chunk(descriptors[i].type, descriptors[i].chunk_index);
    //         }
            
    //         if (chunk->front_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->front_chunk_index].back_chunk_index = NULL_CHUNK_INDEX; }
    //         if (chunk->back_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->back_chunk_index].front_chunk_index = NULL_CHUNK_INDEX; }
    //         if (chunk->top_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->top_chunk_index].bottom_chunk_index = NULL_CHUNK_INDEX; }
    //         if (chunk->bottom_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->bottom_chunk_index].top_chunk_index = NULL_CHUNK_INDEX; }
    //         if (chunk->right_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->right_chunk_index].left_chunk_index = NULL_CHUNK_INDEX; }
    //         if (chunk->left_chunk_index != NULL_CHUNK_INDEX) { chunks[chunk->left_chunk_index].right_chunk_index = NULL_CHUNK_INDEX; }

    //         release_block_pool_chunk(chunk_index);

    //         // Extra check to make sure lol
    //         if (i + 1 >= block_pool.head) {
    //             break;
    //         }

    //         i--; // Keep i fixed since we moved everything around after the releasing of the pool chunk
    //     }
    // }

    remove_chunks_outside_of_range_from_queue(corner_pos, num_procedural_generate_queue_items, procedural_generate_queue);
    remove_chunks_outside_of_range_from_queue(corner_pos, num_visuals_update_queue_items, visuals_update_queue);
}

void manage_block_world(vec3_s32_t last_corner_pos, vec3_s32_t corner_pos) {
    // u16* chunk_indices = block_pool.chunk_indices;
    // block_chunk_t* chunks = block_pool.chunks;



    // size_t gen_index = 0;

    // // Schedule neighborhood updates
    // for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
    //     for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
    //         if (local_chunk_indices[gen_index] != NULL_CHUNK_INDEX) {
    //             gen_index++;
    //             continue;
    //         }

    //         local_update_neighborhood[gen_index] = true;

    //         if (x != (NUM_PER_GENERATION_ROW - 1) && local_chunk_indices[gen_index + X_OFFSET] != NULL_CHUNK_INDEX) {
    //             local_update_neighborhood[gen_index + X_OFFSET] = true;
    //         }

    //         if (x != 0 && local_chunk_indices[gen_index - X_OFFSET] != NULL_CHUNK_INDEX) {
    //             local_update_neighborhood[gen_index - X_OFFSET] = true;
    //         }

    //         if (z != (NUM_PER_GENERATION_ROW - 1) && local_chunk_indices[gen_index + Z_OFFSET] != NULL_CHUNK_INDEX) {
    //             local_update_neighborhood[gen_index + Z_OFFSET] = true;
    //         }

    //         if (z != 0 && local_chunk_indices[gen_index - Z_OFFSET] != NULL_CHUNK_INDEX) {
    //             local_update_neighborhood[gen_index - Z_OFFSET] = true;
    //         }

    //         gen_index++;
    //     }
    // }

    // gen_index = 0;

    // // Allocate new block chunks
    // for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
    //     for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
    //         if (local_chunk_indices[gen_index] != NULL_CHUNK_INDEX) {
    //             gen_index++;
    //             continue;
    //         }

    //         vec3_s32_t pos = {
    //             .x = corner_pos.x + x,
    //             .y = 0,
    //             .z = corner_pos.z + z
    //         };

    //         u16 index = acquire_block_pool_chunk();

    //         positions[index] = pos;
    //         u16 chunk_index = chunk_indices[index];

    //         local_chunk_indices[gen_index] = chunk_index;

    //         block_chunk_t* chunk = &chunks[chunk_index];

    //         memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

    //         chunk->front_chunk_index = NULL_CHUNK_INDEX;
    //         chunk->back_chunk_index = NULL_CHUNK_INDEX;
    //         chunk->top_chunk_index = NULL_CHUNK_INDEX;
    //         chunk->bottom_chunk_index = NULL_CHUNK_INDEX;
    //         chunk->right_chunk_index = NULL_CHUNK_INDEX;
    //         chunk->left_chunk_index = NULL_CHUNK_INDEX;

    //         if (num_procedural_generate_queue_items >= NUM_WORLD_QUEUE_ITEMS) {
    //             // TODO: Implement error logic here
    //             lprintf("Procedural generate queue is full");
    //             for (;;);
    //         }
            
    //         procedural_generate_queue[num_procedural_generate_queue_items++] = (block_chunk_update_t){ chunk_index, pos };

    //         gen_index++;
    //     }
    // }

    // // Update neighborhoods and push visuals updates of block chunks
    // gen_index = 0;
    // for (s32 z = 0; z < NUM_PER_GENERATION_ROW; z++) {
    //     for (s32 x = 0; x < NUM_PER_GENERATION_ROW; x++) {
    //         if (!local_update_neighborhood[gen_index]) {
    //             gen_index++;
    //             continue;
    //         }

    //         u16 chunk_index = local_chunk_indices[gen_index];

    //         vec3_s32_t pos = {
    //             .x = corner_pos.x + x,
    //             .y = 0,
    //             .z = corner_pos.z + z
    //         };

    //         block_chunk_t* chunk = &chunks[chunk_index];

    //         if (x != (NUM_PER_GENERATION_ROW - 1) && chunk->front_chunk_index == NULL_CHUNK_INDEX) {
    //             chunk->front_chunk_index = local_chunk_indices[gen_index + X_OFFSET];
    //         }
    //         if (x != 0 && chunk->back_chunk_index == NULL_CHUNK_INDEX) {
    //             chunk->back_chunk_index = local_chunk_indices[gen_index - X_OFFSET];
    //         }
    //         if (z != (NUM_PER_GENERATION_ROW - 1) && chunk->right_chunk_index == NULL_CHUNK_INDEX) {
    //             chunk->right_chunk_index = local_chunk_indices[gen_index + Z_OFFSET];
    //         }
    //         if (z != 0 && chunk->left_chunk_index == NULL_CHUNK_INDEX) {
    //             chunk->left_chunk_index = local_chunk_indices[gen_index - Z_OFFSET];
    //         }

    //         visuals_update_queue[num_visuals_update_queue_items++] = (block_chunk_update_t){ chunk_index, pos };

    //         gen_index++;
    //     }
    // }
}

void handle_world_queues(vec3_s32_t corner_pos) {
    u8* chunk_indices = block_pool.chunk_indices;
    block_chunk_t* chunks = block_pool.chunks;

    if (num_procedural_generate_queue_items > 0) {
        while (num_procedural_generate_queue_items > 0) {
            vec3_s32_t pos = procedural_generate_queue[--num_procedural_generate_queue_items];

            if (pos.x == UINT32_MAX) {
                continue;
            }

            vec3_s32_t rel_pos = {
                .x = pos.x - corner_pos.x,
                .y = 0,
                .z = pos.z - corner_pos.z
            };
            
            u8 index = (rel_pos.z * Z_OFFSET) + (rel_pos.x * X_OFFSET);
            generate_procedural_blocks(pos, chunks[chunk_indices[index]].blocks);
            break;
        }
    } else if (num_visuals_update_queue_items > 0) { // Bit of a hacky fix, only start dealing with this queue once we don't need to do any procedural generation. This is to allow all procedural generation to finish before visuals updates.
        while (num_visuals_update_queue_items > 0) {
            vec3_s32_t pos = visuals_update_queue[--num_visuals_update_queue_items];

            if (pos.x == UINT32_MAX) {
                continue;
            }

            vec3_s32_t rel_pos = {
                .x = pos.x - corner_pos.x,
                .y = 0,
                .z = pos.z - corner_pos.z
            };

            vec3s world_pos = {
                .x = pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                .y = 20,
                .z = pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK
            };

            u8 index = (rel_pos.z * Z_OFFSET) + (rel_pos.x * X_OFFSET);
            block_chunk_t* chunk = &chunks[chunk_indices[index]];

            memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

            update_block_chunk_visuals(
                world_pos,
                chunk->disp_list_chunk_descriptors,
                chunk->blocks,
                rel_pos.x == NUM_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[index + X_OFFSET]].blocks,
                rel_pos.x == 0 ? NULL : chunks[chunk_indices[index - X_OFFSET]].blocks,
                NULL,
                NULL,
                rel_pos.z == NUM_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[index + Z_OFFSET]].blocks,
                rel_pos.z == 0 ? NULL : chunks[chunk_indices[index - Z_OFFSET]].blocks
            );
            break;
        }
    }
}