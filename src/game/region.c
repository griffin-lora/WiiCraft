#include "region.h"

s32vec3s get_region_position_from_world_position(vec3s world_pos) {
    vec3s div_pos = glms_vec3_scale(world_pos, 1.0f/(f32) REGION_SIZE);
	return (s32vec3s) {{ (s32) floorf(div_pos.x), (s32) floorf(div_pos.y), (s32) floorf(div_pos.z) }};
}