#include "logic.h"
#include "game/region_management.h"
#include "world_location.h"
#include <ogc/lwp_queue.h>
#include <wiiuse/wpad.h>

void update_world_from_location_and_input(s32vec3s region_pos, u32 buttons_down, world_location_t location, vec3s voxel_world_position) {
    (void)region_pos;
    (void)voxel_world_position;
    if (buttons_down & WPAD_BUTTON_A) {
        *location.voxel_type = voxel_type_air;
        // update_block_chunk_and_neighbors(region_pos, location.region_pos, location.voxel_local_pos);
    }
    if (buttons_down & WPAD_BUTTON_B) {
        // world_location_wrap_t normal_offset_loc = get_world_location_at_voxel_world_position(region_pos, voxel_world_position);
        // if (normal_offset_loc.success) {
        //     *normal_offset_loc.val.voxel_type = voxel_type_wood_planks;
            // update_block_chunk_and_neighbors(region_pos, normal_offset_loc.val.region_pos, normal_offset_loc.val.voxel_local_pos);
        // }
    }
}