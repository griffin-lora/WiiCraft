#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"

#include <cstdio>

using namespace game;

glm::vec3 game::get_raycast_direction_from_pointer_position(u16 v_width, u16 v_height, const camera& cam, const std::optional<glm::vec2>& pointer_pos) {
    // if (pointer_pos.has_value()) {
    //     glm::vec2 screen_size = { (f32)v_width, (f32)v_height };
    //     auto relative_pos = *pointer_pos / screen_size;
    //     relative_pos *= 2.f;
    //     relative_pos.x -= 1.f;
    //     relative_pos.y -= 1.f;
    //     relative_pos.y *= -1.f;
    //     // relative_pos is mapped to [-1, 1]

    //     glm::vec3 right = { -cam.look.z, cam.look.y, cam.look.x };
    //     auto up = glm::cross(right, cam.look);

    //     auto fov = glm::radians(cam.fov);
    //     auto fov_tan = tanf(fov / 2.0f);
    //     auto dir = cam.look + (right * (relative_pos.x * fov_tan)) + (up * (relative_pos.y * fov_tan));
        
    //     return glm::normalize(dir);
    // } else {
        return cam.look;
    // }
}

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