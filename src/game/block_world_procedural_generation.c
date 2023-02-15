#include "block_world_procedural_generation.h"
#include "game_math.h"
#include "pool.h"
#include <string.h>

static f32 get_hills_height(vec2s position) {
    position = glms_vec2_scale(position, 1.0f/32.0f);

    return 2.0f * (
        (get_noise_at(position) * 0.4f) +
        (get_noise_at(glms_vec2_scale(position, 3.0f)) * 0.2f) +
        (get_noise_at(glms_vec2_scale(position, 6.0f)) * 0.1f)
    );
}

static f32 get_tallgrass_value(vec2s position) {
    position = glms_vec2_scale(position, 1.0f/2.0f);

    return (
        (get_noise_at(position) * 0.5f) + 
        (get_noise_at(glms_vec2_scale(position, 2.0f)) * 1.0f)
    );
}

static void generate_high_blocks(s32vec3s chunk_pos, block_type_t block_types[]) {
    memset(block_types, block_type_air, 4096);
}

#define NUM_BLOCKS NUM_BLOCKS_PER_BLOCK_CHUNK
#define Z_OFFSET BLOCKS_PER_BLOCK_CHUNK_Z_OFFSET
#define Y_OFFSET BLOCKS_PER_BLOCK_CHUNK_Y_OFFSET
#define X_OFFSET BLOCKS_PER_BLOCK_CHUNK_X_OFFSET

#define NUM_ROW_BLOCKS NUM_ROW_BLOCKS_PER_BLOCK_CHUNK

static void generate_middle_blocks(s32vec3s chunk_pos, block_type_t block_types[]) {
    size_t i = 0;

    f32 world_chunk_x = chunk_pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;
    f32 world_chunk_z = chunk_pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;

    for (f32 z = 0; z < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; z++) {
        for (f32 x = 0; x < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; x++) {
            vec2s noise_pos = { .x = world_chunk_x + x, .y = world_chunk_z + z };

            f32 height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 gen_y = (height * 12) + 1;

            for (s32 y = 0; y < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; y++) {
                if (y > gen_y) {
                    if (y < 7) {
                        block_types[i] = block_type_water;
                    } else if (y == (gen_y + 1) && gen_y >= 7 && tallgrass_value > 0.97f) {
                        block_types[i] = block_type_tall_grass;
                    } else {
                        block_types[i] = block_type_air;
                    }
                } else {
                    if (gen_y < 7) {
                        if (y < (gen_y - 2)) {
                            block_types[i] = block_type_stone;
                        } else {
                            block_types[i] = block_type_sand;
                        }
                    } else if (y < gen_y) {
                        if (y < (gen_y - 2)) {
                            block_types[i] = block_type_stone;
                        } else {
                            block_types[i] = block_type_dirt;
                        }
                    } else {
                        block_types[i] = block_type_grass;
                    }
                }

                i += Y_OFFSET;
            }

            i += X_OFFSET - Z_OFFSET;
        }
        i += Z_OFFSET - Y_OFFSET;
    }
}

static void generate_low_blocks(s32vec3s chunk_pos, block_type_t block_types[]) {
    memset(block_types, block_type_stone, 4096);
}

void generate_procedural_blocks(s32vec3s pos, block_type_t block_types[]) {
    if (pos.y > 0) {
        generate_high_blocks(pos, block_types);
    } else if (pos.y < 0) {
        generate_low_blocks(pos, block_types);
    } else {
        generate_middle_blocks(pos, block_types);
    }
}