#include "region.h"
#include "game_math.h"
#include "util.h"

s32vec3s get_region_position_from_voxel_world_position(s32vec3s voxel_world_pos) {
	return (s32vec3s) {{ div_s32(voxel_world_pos.x, REGION_SIZE), div_s32(voxel_world_pos.y, REGION_SIZE), div_s32(voxel_world_pos.z, REGION_SIZE) }};
}