#include "voxel.h"
#include "game/region.h"
#include "game_math.h"
#include "util.h"

s32vec3s get_voxel_world_position(vec3s world_pos) {
    return (s32vec3s) {{ (s32) floorf(world_pos.x), (s32) floorf(world_pos.y), (s32) floorf(world_pos.z) }};
}

u32vec3s get_voxel_local_position_from_voxel_world_position(s32vec3s voxel_world_pos) {
    return (u32vec3s) {{
        (u32) mod_s32(voxel_world_pos.x, REGION_SIZE),
        (u32) mod_s32(voxel_world_pos.y, REGION_SIZE),
        (u32) mod_s32(voxel_world_pos.z, REGION_SIZE),
    }};
}