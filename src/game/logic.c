#include "logic.h"
#include "chunk_core.h"
#include <wiiuse/wpad.h>

void update_world_from_raycast_and_input(s32vec3s corner_pos, u32 buttons_down, world_location_t location, vec3s offset_world_block_position) {
    if (buttons_down & WPAD_BUTTON_A) {
        *location.bl_tp = block_type_air;
        update_block_chunk_and_neighbors(corner_pos, location.ch_pos, location.bl_pos);
    }
    if (buttons_down & WPAD_BUTTON_B) {
        world_location_wrap_t normal_offset_loc = get_world_location_at_world_position(corner_pos, offset_world_block_position);
        if (normal_offset_loc.success) {
            *normal_offset_loc.val.bl_tp = block_type_wood_planks;
            update_block_chunk_and_neighbors(corner_pos, normal_offset_loc.val.ch_pos, normal_offset_loc.val.bl_pos);
        }
    }
}