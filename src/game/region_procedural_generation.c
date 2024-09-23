#include "region_procedural_generation.h"
#include "game/region.h"
#include "game/voxel.h"
#include "game_math.h"
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

static void generate_high_voxels(s32vec3s, voxel_type_array_t* voxel_types) {
    memset(voxel_types->types, voxel_type_air, 4096);
}

voxel_type_t get_voxel_type_at_position(s32 y, s32 gen_y, f32 tallgrass_value) {
    if (y > gen_y) {
        if (y < 7) {
            return voxel_type_water;
        }
        if (y == (gen_y + 1) && gen_y >= 7 && tallgrass_value > 0.97f) {
            return voxel_type_tall_grass;
        }
        return voxel_type_air;
    }
    if (gen_y < 7) {
        if (y < (gen_y - 2)) {
            return voxel_type_stone;
        }
        return voxel_type_sand;
    }
    if (y < gen_y) {
        if (y < (gen_y - 2)) {
            return voxel_type_stone;
        }
        return voxel_type_dirt;
    }
    return voxel_type_grass;
}

static void generate_middle_voxels(s32vec3s region_pos, voxel_type_array_t* voxel_types) {
    f32 x_offset = (f32) region_pos.x * REGION_SIZE;
    f32 world_region_z = (f32) region_pos.z * REGION_SIZE;

    for (s32 x = 0; x < REGION_SIZE; x++) {
        for (s32 z = 0; z < REGION_SIZE; z++) {
            vec2s noise_pos = { .x = x_offset + (f32) x, .y = world_region_z + (f32) z };

            f32 height = get_hills_height(noise_pos);
            
            f32 tallgrass_value = get_tallgrass_value(noise_pos);

            s32 gen_y = (s32) (height * 12) + 1;

            for (s32 y = 0; y < REGION_SIZE; y++) {
                voxel_type_t* type = &voxel_types->types[(size_t) x][(size_t) y][(size_t) z];
                *type = get_voxel_type_at_position(y, gen_y, tallgrass_value);
            }
        }
    }
}

static void generate_low_voxels(s32vec3s, voxel_type_array_t* voxel_types) {
    memset(voxel_types->types, voxel_type_stone, 4096);
}

void generate_region_voxels(s32vec3s pos, voxel_type_array_t* voxel_types) {
    if (pos.y > 0) {
        generate_high_voxels(pos, voxel_types);
    } else if (pos.y < 0) {
        generate_low_voxels(pos, voxel_types);
    } else {
        generate_middle_voxels(pos, voxel_types);
    }
}