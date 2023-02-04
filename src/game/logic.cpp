#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"

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

void game::update_world_from_raycast_and_input(chunk::map& chunks, u32 buttons_down, block_raycast_wrap_t& raycast) {
    // TODO: Ugly garbage code. Refactor.
    if (raycast.success) {
        if (buttons_down & WPAD_BUTTON_A) {
            get_block_count_ref(*raycast.val.location.ch, *raycast.val.location.bl)--;
            *raycast.val.location.bl = { .tp = block::type::air };
            get_block_count_ref(*raycast.val.location.ch, *raycast.val.location.bl)++;
            raycast.val.location.ch->modified = true;
            add_important_chunk_mesh_update(*raycast.val.location.ch, raycast.val.location.bl_pos);
        }
        if (buttons_down & WPAD_BUTTON_B) {
            auto normal_offset_loc = get_world_location_at_world_position(chunks, raycast.val.world_block_position + raycast.val.box_raycast.normal);
            if (normal_offset_loc.has_value()) {
                get_block_count_ref(*normal_offset_loc->ch, *normal_offset_loc->bl)--;
                *normal_offset_loc->bl = { .tp = block::type::stone_slab_bottom };
                get_block_count_ref(*normal_offset_loc->ch, *normal_offset_loc->bl)++;
                normal_offset_loc->ch->modified = true;
                add_important_chunk_mesh_update(*normal_offset_loc->ch, normal_offset_loc->bl_pos);
            }
        }
    }
}