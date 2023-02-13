#include "block_world_procedural_generation.hpp"
#include "math.hpp"
#include "pool.hpp"
#include <string.h>

using math::get_noise_at;

static f32 get_hills_height(glm::vec2 position) {
    position /= 32.0f;

    return 2.0f * (
        (get_noise_at(position) * 0.4f) +
        (get_noise_at(position * 3.0f) * 0.2f) +
        (get_noise_at(position * 6.0f) * 0.1f)
    );
}

static f32 get_tallgrass_value(glm::vec2 position) {
    position /= 2.0f;

    return (
        (get_noise_at(position) * 0.5f) + 
        (get_noise_at(position * 2.0f) * 1.0f)
    );
}

static void generate_high_blocks(math::vector3s32 chunk_pos, block_type_t block_types[]) {
    memset(block_types, block_type_air, 4096);
}

static constexpr s32 z_offset = NUM_ROW_BLOCKS_PER_BLOCK_CHUNK * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;
static constexpr s32 y_offset = NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;
static constexpr s32 x_offset = 1;

static void generate_middle_blocks(math::vector3s32 chunk_pos, block_type_t block_types[]) {
    auto it = block_types;

    f32 world_chunk_x = chunk_pos.x * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;
    f32 world_chunk_z = chunk_pos.z * NUM_ROW_BLOCKS_PER_BLOCK_CHUNK;

    for (f32 z = 0; z < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; z++) {
        for (f32 x = 0; x < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; x++) {
            f32 world_x = world_chunk_x + x;
            f32 world_z = world_chunk_z + z;
            glm::vec2 noise_pos = { world_x, world_z };

            f32 height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 gen_y = (height * 12) + 1;

            for (s32 y = 0; y < NUM_ROW_BLOCKS_PER_BLOCK_CHUNK; y++) {
                if (y > gen_y) {
                    if (y < 7) {
                        *it = block_type_water;
                    } else if (y == (gen_y + 1) && gen_y >= 7 && tallgrass_value > 0.97f) {
                        *it = block_type_tall_grass;
                    } else {
                        *it = block_type_air;
                    }
                } else {
                    if (gen_y < 7) {
                        if (y < (gen_y - 2)) {
                            *it = block_type_stone;
                        } else {
                            *it = block_type_sand;
                        }
                    } else if (y < gen_y) {
                        if (y < (gen_y - 2)) {
                            *it = block_type_stone;
                        } else {
                            *it = block_type_dirt;
                        }
                    } else {
                        *it = block_type_grass;
                    }
                }

                it += y_offset;
            }

            it += x_offset - z_offset;
        }
        it += z_offset - y_offset;
    }
}

static void generate_low_blocks(math::vector3s32 chunk_pos, block_type_t block_types[]) {
    memset(block_types, block_type_stone, 4096);
}

void generate_procedural_blocks(vec3_s32_t pos, block_type_t block_types[]) {
    math::vector3s32 chunk_pos = { pos.x, pos.y, pos.z };
    if (chunk_pos.y > 0) {
        generate_high_blocks(chunk_pos, block_types);
    } else if (chunk_pos.y < 0) {
        generate_low_blocks(chunk_pos, block_types);
    } else {
        generate_middle_blocks(chunk_pos, block_types);
    }
}