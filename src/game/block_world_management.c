#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include "log.h"
#include "util.h"
#include <stdbool.h>
#include <string.h>

#define Z_OFFSET BLOCK_POOL_CHUNK_INDICES_Z_OFFSET
#define Y_OFFSET BLOCK_POOL_CHUNK_INDICES_Y_OFFSET
#define X_OFFSET BLOCK_POOL_CHUNK_INDICES_X_OFFSET

size_t num_procedural_generate_queue_items = 0;
_Alignas(32) vec3_s32_t procedural_generate_queue[NUM_WORLD_QUEUE_ITEMS];

size_t num_visuals_update_queue_items = 0;
_Alignas(32) vec3_s32_t visuals_update_queue[NUM_WORLD_QUEUE_ITEMS];

static void remove_chunks_outside_of_range_from_queue(vec3_s32_t corner_pos, size_t num_items, vec3_s32_t queue[]) {
    for (size_t i = 0; i < num_items; i++) {
        vec3_s32_t pos = queue[i];
        vec3_s32_t rel_pos = { pos.x - corner_pos.x, pos.y - corner_pos.y, pos.z - corner_pos.z };
        if (rel_pos.x >= 0 && rel_pos.x < NUM_XZ_ROW_BLOCK_CHUNKS && rel_pos.y >= 0 && rel_pos.y < NUM_Y_ROW_BLOCK_CHUNKS && rel_pos.z >= 0 && rel_pos.z < NUM_XZ_ROW_BLOCK_CHUNKS) {
            continue;
        }

        queue[i].x = INT32_MAX;
    }
}

void init_block_world(vec3_s32_t corner_pos) {
    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                vec3_s32_t update_pos = {
                    .x = x + corner_pos.x,
                    .y = y + corner_pos.y,
                    .z = z + corner_pos.z
                };

                block_chunk_t* chunk = &block_pool_chunks[block_pool_chunk_indices[i]];

                memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

                chunk->has_trivial_visuals = update_pos.y != 0;

                procedural_generate_queue[num_procedural_generate_queue_items++] = update_pos;
                visuals_update_queue[num_visuals_update_queue_items++] = update_pos;
            }
        }
    }
}

_Alignas(32) u8 temp_block_pool_chunk_indices[NUM_BLOCK_CHUNKS];

void manage_block_world(vec3_s32_t last_corner_pos, vec3_s32_t corner_pos) {
    remove_chunks_outside_of_range_from_queue(corner_pos, num_procedural_generate_queue_items, procedural_generate_queue);
    remove_chunks_outside_of_range_from_queue(corner_pos, num_visuals_update_queue_items, visuals_update_queue);

    vec3_s32_t move_dir = {
        .x = last_corner_pos.x - corner_pos.x,
        .y = last_corner_pos.y - corner_pos.y,
        .z = last_corner_pos.z - corner_pos.z
    };

    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                size_t chunk_index = block_pool_chunk_indices[i];

                vec3_s32_t new_pos = {
                    .x = x + move_dir.x,
                    .y = y + move_dir.y,
                    .z = z + move_dir.z
                };

                if (new_pos.x < 0 || new_pos.x >= NUM_XZ_ROW_BLOCK_CHUNKS || new_pos.y < 0 || new_pos.y >= NUM_Y_ROW_BLOCK_CHUNKS || new_pos.z < 0 || new_pos.z >= NUM_XZ_ROW_BLOCK_CHUNKS) {
                    new_pos.x = mod_s32(new_pos.x, NUM_XZ_ROW_BLOCK_CHUNKS);
                    new_pos.y = mod_s32(new_pos.y, NUM_Y_ROW_BLOCK_CHUNKS);
                    new_pos.z = mod_s32(new_pos.z, NUM_XZ_ROW_BLOCK_CHUNKS);

                    vec3_s32_t update_pos = {
                        .x = new_pos.x + corner_pos.x,
                        .y = new_pos.y + corner_pos.y,
                        .z = new_pos.z + corner_pos.z
                    };

                    // Erase the old chunk and add a new one
                    block_chunk_t* chunk = &block_pool_chunks[chunk_index];

                    block_display_list_chunk_descriptor_t* descriptors = chunk->disp_list_chunk_descriptors;
                    for (size_t i = 0; descriptors[i].type != 0xff; i++) {
                        release_block_display_list_pool_chunk(descriptors[i].type, descriptors[i].chunk_index);
                    }

                    memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

                    chunk->has_trivial_visuals = update_pos.y != 0;

                    if (num_procedural_generate_queue_items >= NUM_WORLD_QUEUE_ITEMS || num_visuals_update_queue_items >= NUM_WORLD_QUEUE_ITEMS) {
                        lprintf("Too many queue items\n");
                        for (;;);
                    }

                    procedural_generate_queue[num_procedural_generate_queue_items++] = update_pos;
                    visuals_update_queue[num_visuals_update_queue_items++] = update_pos;
                } else {
                    vec3_s32_t neighbor_pos = {
                        .x = x - move_dir.x,
                        .y = y - move_dir.y,
                        .z = z - move_dir.z
                    };
                    
                    if (neighbor_pos.x < 0 || neighbor_pos.x >= NUM_XZ_ROW_BLOCK_CHUNKS || neighbor_pos.y < 0 || neighbor_pos.y >= NUM_Y_ROW_BLOCK_CHUNKS || neighbor_pos.z < 0 || neighbor_pos.z >= NUM_XZ_ROW_BLOCK_CHUNKS) {
                        // Update because our neighbor is new
                        vec3_s32_t update_pos = {
                            .x = new_pos.x + corner_pos.x,
                            .y = new_pos.y + corner_pos.y,
                            .z = new_pos.z + corner_pos.z
                        };

                        if (num_procedural_generate_queue_items >= NUM_WORLD_QUEUE_ITEMS || num_visuals_update_queue_items >= NUM_WORLD_QUEUE_ITEMS) {
                            lprintf("Too many queue items\n");
                            for (;;);
                        }

                        procedural_generate_queue[num_procedural_generate_queue_items++] = update_pos;
                        visuals_update_queue[num_visuals_update_queue_items++] = update_pos;
                    }
                }

                temp_block_pool_chunk_indices[(new_pos.z * Z_OFFSET) + (new_pos.y * Y_OFFSET) + (new_pos.x * X_OFFSET)] = chunk_index;
            }
        }
    }

    // Set the real indices
    for (size_t i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        block_pool_chunk_indices[i] = temp_block_pool_chunk_indices[i];
    }
}

void handle_world_queues(vec3_s32_t corner_pos) {
    u8* chunk_indices = block_pool_chunk_indices;
    block_chunk_t* chunks = block_pool_chunks;

    if (num_procedural_generate_queue_items > 0) {
        while (num_procedural_generate_queue_items > 0) {
            vec3_s32_t pos = procedural_generate_queue[--num_procedural_generate_queue_items];

            if (pos.x == INT32_MAX) {
                continue;
            }

            vec3_s32_t rel_pos = {
                .x = pos.x - corner_pos.x,
                .y = pos.y - corner_pos.y,
                .z = pos.z - corner_pos.z
            };
            
            size_t index = (rel_pos.z * Z_OFFSET) + (rel_pos.y * Y_OFFSET) + (rel_pos.x * X_OFFSET);
            generate_procedural_blocks(pos, chunks[chunk_indices[index]].blocks);

            if (pos.y == 0) {
                break;
            }
        }
    } else if (num_visuals_update_queue_items > 0) { // Bit of a hacky fix, only start dealing with this queue once we don't need to do any procedural generation. This is to allow all procedural generation to finish before visuals updates.
        while (num_visuals_update_queue_items > 0) {
            vec3_s32_t pos = visuals_update_queue[--num_visuals_update_queue_items];

            if (pos.x == INT32_MAX) {
                continue;
            }

            vec3_s32_t rel_pos = {
                .x = pos.x - corner_pos.x,
                .y = pos.y - corner_pos.y,
                .z = pos.z - corner_pos.z
            };

            size_t index = (rel_pos.z * Z_OFFSET) + (rel_pos.y * Y_OFFSET) + (rel_pos.x * X_OFFSET);
            block_chunk_t* chunk = &chunks[chunk_indices[index]];

            if (chunk->has_trivial_visuals) {
                continue;
            }

            vec3s world_pos = {
                .x = pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                .y = pos.y * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                .z = pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK
            };

            update_block_chunk_visuals(
                world_pos,
                chunk->disp_list_chunk_descriptors,
                chunk->blocks,
                rel_pos.x == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[index + X_OFFSET]].blocks,
                rel_pos.x == 0 ? NULL : chunks[chunk_indices[index - X_OFFSET]].blocks,
                rel_pos.y == NUM_Y_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[index + Y_OFFSET]].blocks,
                rel_pos.y == 0 ? NULL : chunks[chunk_indices[index - Y_OFFSET]].blocks,
                rel_pos.z == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[index + Z_OFFSET]].blocks,
                rel_pos.z == 0 ? NULL : chunks[chunk_indices[index - Z_OFFSET]].blocks
            );

            break;
        }
    }
}