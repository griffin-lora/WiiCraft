#include "block_world_management.h"
#include "block_world_mesh_generation.h"
#include "block_world_procedural_generation.h"
#include "pool.h"
#include "log.h"
#include "util.h"
#include "chrono.h"
#include <stdbool.h>
#include <string.h>

#define Z_OFFSET BLOCK_POOL_CHUNK_INDICES_Z_OFFSET
#define Y_OFFSET BLOCK_POOL_CHUNK_INDICES_Y_OFFSET
#define X_OFFSET BLOCK_POOL_CHUNK_INDICES_X_OFFSET

void init_block_world(s32vec3s corner_pos) {
    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                block_pool_chunk_bitfields[i] |= BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED;
                if (y + corner_pos.y != 0) {
                    block_pool_chunk_bitfields[i] |= BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS;
                }

                block_chunk_t* chunk = &block_pool_chunks[block_pool_chunk_indices[i]];

                memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));
            }
        }
    }
}

_Alignas(32) u8 temp_block_pool_chunk_indices[NUM_BLOCK_CHUNKS];
_Alignas(32) u8 temp_block_pool_chunk_bitfields[NUM_BLOCK_CHUNKS];

void manage_block_world(s32vec3s last_corner_pos, s32vec3s corner_pos) {
    s32vec3s move_dir = {
        .x = last_corner_pos.x - corner_pos.x,
        .y = last_corner_pos.y - corner_pos.y,
        .z = last_corner_pos.z - corner_pos.z
    };

    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                size_t chunk_index = block_pool_chunk_indices[i];
                size_t chunk_bitfield = block_pool_chunk_bitfields[i];

                s32vec3s new_pos = {
                    .x = x + move_dir.x,
                    .y = y + move_dir.y,
                    .z = z + move_dir.z
                };

                if (new_pos.x < 0 || new_pos.x >= NUM_XZ_ROW_BLOCK_CHUNKS || new_pos.y < 0 || new_pos.y >= NUM_Y_ROW_BLOCK_CHUNKS || new_pos.z < 0 || new_pos.z >= NUM_XZ_ROW_BLOCK_CHUNKS) {
                    new_pos.x = mod_s32(new_pos.x, NUM_XZ_ROW_BLOCK_CHUNKS);
                    new_pos.y = mod_s32(new_pos.y, NUM_Y_ROW_BLOCK_CHUNKS);
                    new_pos.z = mod_s32(new_pos.z, NUM_XZ_ROW_BLOCK_CHUNKS);

                    // Erase the old chunk and add a new one
                    block_chunk_t* chunk = &block_pool_chunks[chunk_index];

                    block_display_list_chunk_descriptor_t* descriptors = chunk->disp_list_chunk_descriptors;
                    for (size_t i = 0; i < 16 && descriptors[i].type != 0xff; i++) {
                        if (!release_block_display_list_pool_chunk(descriptors[i].type, descriptors[i].chunk_index)) {
                            lprintf("block_world_management.c\n");
                        }
                    }

                    memset(chunk->disp_list_chunk_descriptors, 0xff, sizeof(chunk->disp_list_chunk_descriptors));

                    chunk_bitfield = 0;
                    chunk_bitfield |= BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED;
                    if (new_pos.y + corner_pos.y != 0) {
                        chunk_bitfield |= BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS;
                    }
                } else {
                    s32vec3s neighbor_pos = {
                        .x = x - move_dir.x,
                        .y = y - move_dir.y,
                        .z = z - move_dir.z
                    };
                    
                    if (neighbor_pos.x < 0 || neighbor_pos.x >= NUM_XZ_ROW_BLOCK_CHUNKS || neighbor_pos.y < 0 || neighbor_pos.y >= NUM_Y_ROW_BLOCK_CHUNKS || neighbor_pos.z < 0 || neighbor_pos.z >= NUM_XZ_ROW_BLOCK_CHUNKS) {
                        // Update because our neighbor is new
                        chunk_bitfield |= BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED;
                    }
                }

                size_t index = (new_pos.z * Z_OFFSET) + (new_pos.y * Y_OFFSET) + (new_pos.x * X_OFFSET);
                temp_block_pool_chunk_indices[index] = chunk_index;
                temp_block_pool_chunk_bitfields[index] = chunk_bitfield;
            }
        }
    }

    // Set the real indices
    for (size_t i = 0; i < NUM_BLOCK_CHUNKS; i++) {
        block_pool_chunk_indices[i] = temp_block_pool_chunk_indices[i];
        block_pool_chunk_bitfields[i] = temp_block_pool_chunk_bitfields[i];
    }
}

static bool handle_procedural_generation(s32vec3s corner_pos) {
    u8* chunk_indices = block_pool_chunk_indices;
    u8* chunk_bitfields = block_pool_chunk_bitfields;
    block_chunk_t* chunks = block_pool_chunks;

    size_t num_procedural_generate_items = 0;

    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                if (chunk_bitfields[i] & BLOCK_CHUNK_FLAG_HAS_VALID_BLOCKS) {
                    continue;
                }

                lprintf("Procedural Generation %d, %d, %d\n", x, y, z);

                chunk_bitfields[i] |= BLOCK_CHUNK_FLAG_HAS_VALID_BLOCKS;

                s32vec3s pos = {
                    .x = x + corner_pos.x,
                    .y = y + corner_pos.y,
                    .z = z + corner_pos.z
                };

                s64 start = get_current_us();
                generate_procedural_blocks(pos, chunks[chunk_indices[i]].block_types);
                total_block_gen_time += get_current_us() - start;

                if (pos.y != 0) {
                    return true;
                }

                num_procedural_generate_items++;
            }
        }
    }

    return num_procedural_generate_items > 0;
}

static void handle_important_visuals_updating(s32vec3s corner_pos) {
    u8* chunk_indices = block_pool_chunk_indices;
    u8* chunk_bitfields = block_pool_chunk_bitfields;
    block_chunk_t* chunks = block_pool_chunks;

    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                u8 flags = chunk_bitfields[i];
                if (!(flags & BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT)) {
                    continue;
                }

                chunk_bitfields[i] &= (~BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT);
                // Don't get rid of queued visuals update since sometimes there is a good reason for it to update again

                lprintf("Updating visuals important %d, %d, %d\n", x, y, z);

                block_chunk_t* chunk = &chunks[chunk_indices[i]];

                s32vec3s pos = {
                    .x = x + corner_pos.x,
                    .y = y + corner_pos.y,
                    .z = z + corner_pos.z
                };

                vec3s world_pos = {
                    .x = pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                    .y = pos.y * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                    .z = pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK
                };

                s64 start = get_current_us();
                update_block_chunk_visuals(
                    world_pos,
                    chunk->disp_list_chunk_descriptors,
                    chunk->block_types,
                    x == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + X_OFFSET]].block_types,
                    x == 0 ? NULL : chunks[chunk_indices[i - X_OFFSET]].block_types,
                    y == NUM_Y_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + Y_OFFSET]].block_types,
                    y == 0 ? NULL : chunks[chunk_indices[i - Y_OFFSET]].block_types,
                    z == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + Z_OFFSET]].block_types,
                    z == 0 ? NULL : chunks[chunk_indices[i - Z_OFFSET]].block_types
                );
                us_t time = get_current_us() - start;
                total_mesh_gen_time += time;
                last_mesh_gen_time = time;
            }
        }
    }
}

static void handle_queued_visuals_updating(s32vec3s corner_pos) {
    u8* chunk_indices = block_pool_chunk_indices;
    u8* chunk_bitfields = block_pool_chunk_bitfields;
    block_chunk_t* chunks = block_pool_chunks;

    size_t i = 0;
    for (s32 z = 0; z < NUM_XZ_ROW_BLOCK_CHUNKS; z++) {
        for (s32 y = 0; y < NUM_Y_ROW_BLOCK_CHUNKS; y++) {
            for (s32 x = 0; x < NUM_XZ_ROW_BLOCK_CHUNKS; x++, i++) {
                u8 flags = chunk_bitfields[i];
                if (!(flags & BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED)) {
                    continue;
                }

                chunk_bitfields[i] &= (~BLOCK_CHUNK_FLAG_UPDATE_VISUALS_IMPORTANT);
                chunk_bitfields[i] &= (~BLOCK_CHUNK_FLAG_UPDATE_VISUALS_QUEUED);

                if (flags & BLOCK_CHUNK_FLAG_HAS_TRIVIAL_VISUALS) {
                    continue;
                }

                lprintf("Updating visuals %d, %d, %d\n", x, y, z);

                block_chunk_t* chunk = &chunks[chunk_indices[i]];

                s32vec3s pos = {
                    .x = x + corner_pos.x,
                    .y = y + corner_pos.y,
                    .z = z + corner_pos.z
                };

                vec3s world_pos = {
                    .x = pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                    .y = pos.y * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK,
                    .z = pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK
                };

                s64 start = get_current_us();
                update_block_chunk_visuals(
                    world_pos,
                    chunk->disp_list_chunk_descriptors,
                    chunk->block_types,
                    x == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + X_OFFSET]].block_types,
                    x == 0 ? NULL : chunks[chunk_indices[i - X_OFFSET]].block_types,
                    y == NUM_Y_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + Y_OFFSET]].block_types,
                    y == 0 ? NULL : chunks[chunk_indices[i - Y_OFFSET]].block_types,
                    z == NUM_XZ_ROW_BLOCK_CHUNKS - 1 ? NULL : chunks[chunk_indices[i + Z_OFFSET]].block_types,
                    z == 0 ? NULL : chunks[chunk_indices[i - Z_OFFSET]].block_types
                );
                us_t time = get_current_us() - start;
                total_mesh_gen_time += time;
                last_mesh_gen_time = time;

                return;
            }
        }
    }
}

void handle_world_flag_processing(s32vec3s corner_pos) {
    bool any_procedural_generation = handle_procedural_generation(corner_pos);
    handle_important_visuals_updating(corner_pos);
    if (!any_procedural_generation) { // Only handle queued visuals updating if we did not see any procedural generation
        handle_queued_visuals_updating(corner_pos);
    }
}