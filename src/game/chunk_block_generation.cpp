#include "chunk_block_generation.hpp"
#include "chunk_core.hpp"
#include <string.h>

using namespace game;
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

static void generate_high_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    memset(chunk.blocks, block_type_air, 4096);
    get_block_count_ref(chunk, block_type_air) = 4096;
}

static constexpr s32 z_offset = chunk::size * chunk::size;
static constexpr s32 y_offset = chunk::size;
static constexpr s32 x_offset = 1;

static void generate_middle_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    auto it = chunk.blocks;

    f32 world_chunk_x = chunk_pos.x * chunk::size;
    f32 world_chunk_z = chunk_pos.z * chunk::size;

    for (f32 z = 0; z < chunk::size; z++) {
        for (f32 x = 0; x < chunk::size; x++) {
            f32 world_x = world_chunk_x + x;
            f32 world_z = world_chunk_z + z;
            glm::vec2 noise_pos = { world_x, world_z };

            f32 height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 gen_y = (height * 12) + 1;

            for (s32 y = 0; y < chunk::size; y++) {
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

                get_block_count_ref(chunk, *it)++;

                it += y_offset;
            }

            it += x_offset - z_offset;
        }
        it += z_offset - y_offset;
    }
}

static void generate_low_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    memset(chunk.blocks, block_type_stone, 4096);
    get_block_count_ref(chunk, block_type_stone) = 4096;
}

mesh_update_state game::generate_blocks(chunk& chunk, const math::vector3s32& chunk_pos) {
    if (chunk_pos.y > 0) {
        generate_high_blocks(chunk, chunk_pos);
        return mesh_update_state::should_continue;
    } else if (chunk_pos.y < 0) {
        generate_low_blocks(chunk, chunk_pos);
        return mesh_update_state::should_continue;
    } else {
        generate_middle_blocks(chunk, chunk_pos);
        return mesh_update_state::should_break;
    }
}