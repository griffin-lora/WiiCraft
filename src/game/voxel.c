#include "voxel.h"
#include "game/region.h"
#include "game_math.h"
#include "util.h"

u32vec3s get_voxel_local_position_from_world_position(vec3s world_pos) {
    return (u32vec3s) {{
        (u32) mod_s32((s32) floorf(world_pos.x), REGION_SIZE),
        (u32) mod_s32((s32) floorf(world_pos.y), REGION_SIZE),
        (u32) mod_s32((s32) floorf(world_pos.z), REGION_SIZE),
    }};
}