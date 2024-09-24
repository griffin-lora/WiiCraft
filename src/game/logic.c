#include "logic.h"
#include "game/region.h"
#include "game/region_management.h"
#include "game/voxel.h"
#include "game/voxel_raycast.h"
#include "game_math.h"
#include "util.h"
#include <ogc/lwp_queue.h>
#include <wiiuse/wpad.h>

void update_world(const voxel_raycast_t* raycast, u32 buttons_down) {
    if (buttons_down & WPAD_BUTTON_A) {
        *get_voxel_type_from_voxel_world_position(raycast->voxel_world_pos) = voxel_type_air;
        // update_block_chunk_and_neighbors(region_pos, location.region_pos, location.voxel_local_pos);
    }
    if (buttons_down & WPAD_BUTTON_B) {
        s32vec3s voxel_world_pos = raycast->voxel_world_pos;
        voxel_world_pos.x += (s32) raycast->box_raycast.normal.x;
        voxel_world_pos.y += (s32) raycast->box_raycast.normal.y;
        voxel_world_pos.z += (s32) raycast->box_raycast.normal.z;

        voxel_type_t* voxel_type = get_voxel_type_from_voxel_world_position(voxel_world_pos);
        if (voxel_type == NULL) {
            return;
        }

        *voxel_type = voxel_type_wood_planks;
    }
}