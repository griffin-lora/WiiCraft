#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"

#include <cstdio>

using namespace game;

void game::update_world_from_raycast_and_input(chunk::map& chunks, u32 buttons_down, std::optional<block_raycast>& raycast) {
    // TODO: Ugly garbage code. Refactor.
    if (raycast.has_value()) {
        if (buttons_down & WPAD_BUTTON_A) {
            *raycast->location.bl = { .tp = block::type::AIR };
            raycast->location.ch->modified = true;
            add_important_chunk_mesh_update(*raycast->location.ch, raycast->location.bl_pos);
        }
        if (buttons_down & WPAD_BUTTON_B) {
            if (raycast->box_raycast.normal.y == 1.0f && raycast->location.bl->tp == block::type::STONE_SLAB && raycast->location.bl->st.slab == block::slab_state::BOTTOM) {
                *raycast->location.bl = { .tp = block::type::STONE_SLAB, .st = { .slab = block::slab_state::BOTH } };
                raycast->location.ch->modified = true;
                add_important_chunk_mesh_update(*raycast->location.ch, raycast->location.bl_pos);
            } else {
                auto normal_offset_loc = get_world_location_at_world_position(chunks, raycast->world_block_position + raycast->box_raycast.normal);
                if (normal_offset_loc.has_value()) {
                    *normal_offset_loc->bl = { .tp = block::type::STONE_SLAB, .st = { .slab = block::slab_state::BOTTOM } };
                    normal_offset_loc->ch->modified = true;
                    add_important_chunk_mesh_update(*normal_offset_loc->ch, normal_offset_loc->bl_pos);
                }
            }
        }
    }
}