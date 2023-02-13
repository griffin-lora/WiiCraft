#include "logic.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"
#include "block_world_management.hpp"

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

void game::update_world_from_raycast_and_input(vec3_s32_t corner_pos, u32 buttons_down, block_raycast_wrap_t& raycast) {
    // TODO: Ugly garbage code. Refactor.
    if (raycast.success) {
        if (buttons_down & WPAD_BUTTON_A) {
            *raycast.val.location.bl_tp = block_type_air;
            auto ch_pos = raycast.val.location.ch_pos;
            visuals_update_queue[num_visuals_update_queue_items++] = { ch_pos.x, 0, ch_pos.z };
        }
        if (buttons_down & WPAD_BUTTON_B) {
            auto normal_offset_loc = get_world_location_at_world_position(corner_pos, raycast.val.world_block_position + raycast.val.box_raycast.normal);
            if (normal_offset_loc.success) {
                *normal_offset_loc.val.bl_tp = block_type_wood_planks;
                auto ch_pos = normal_offset_loc.val.ch_pos;
                visuals_update_queue[num_visuals_update_queue_items++] = { ch_pos.x, 0, ch_pos.z };
            }
        }
    }
}