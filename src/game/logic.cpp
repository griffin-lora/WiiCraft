#include "logic.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"
#include "block_world_management.hpp"

using namespace game;

void game::update_world_from_raycast_and_input(s32vec3s corner_pos, u32 buttons_down, block_raycast_wrap_t& raycast) {
    // TODO: Ugly garbage code. Refactor.
    if (raycast.success) {
        if (buttons_down & WPAD_BUTTON_A) {
            *raycast.val.location.bl_tp = block_type_air;
            update_block_chunk_and_neighbors(corner_pos, raycast.val.location.ch_pos, raycast.val.location.bl_pos);
        }
        if (buttons_down & WPAD_BUTTON_B) {
            auto normal_offset_loc = get_world_location_at_world_position(corner_pos, glms_vec3_add(raycast.val.world_block_position, raycast.val.box_raycast.normal));
            if (normal_offset_loc.success) {
                *normal_offset_loc.val.bl_tp = block_type_wood_planks;
                update_block_chunk_and_neighbors(corner_pos, normal_offset_loc.val.ch_pos, normal_offset_loc.val.bl_pos);
            }
        }
    }
}