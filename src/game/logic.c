#include "logic.h"
#include "game/camera.h"
#include "game/voxel_raycast.h"
#include "game/voxel_selection.h"
#include "log.h"
#include <ogc/lwp_queue.h>
#include <wiiuse/wpad.h>

void update_world(const mat4s* view, const voxel_raycast_t* raycast, u32 buttons_down) {
    (void) buttons_down;
    (void) view;
    (void) raycast;

    // if (buttons_down & WPAD_BUTTON_A) {
    //     *location.voxel_type = voxel_type_air;
    //     update_block_chunk_and_neighbors(region_pos, location.region_pos, location.voxel_local_pos);
    // }
    // if (buttons_down & WPAD_BUTTON_B) {
    //     world_location_wrap_t normal_offset_loc = get_world_location_at_voxel_world_position(region_pos, voxel_world_position);
    //     if (normal_offset_loc.success) {
    //         *normal_offset_loc.val.voxel_type = voxel_type_wood_planks;
    //         update_block_chunk_and_neighbors(region_pos, normal_offset_loc.val.region_pos, normal_offset_loc.val.voxel_local_pos);
    //     }
    // }
}