#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

std::optional<raycast> game::get_raycast(const camera& cam, chunk::map& chunks) {
    glm::vec3 raycast_pos = cam.position;
    glm::vec3 dir_vec = cam.look * 0.5f;
    std::optional<math::vector3s32> current_chunk_pos = {};
    chunk::opt_ref current_chunk = {};
    for (u8 i = 0; i < 40; i++) {
        auto raycast_chunk_pos = get_chunk_position_from_world_position(raycast_pos);

        if (current_chunk_pos.has_value()) {
            if (raycast_chunk_pos != current_chunk_pos.value()) {
                if (!chunks.count(raycast_chunk_pos)) {
                    break;
                }
                current_chunk_pos = raycast_chunk_pos;
                current_chunk = chunks.at(raycast_chunk_pos);
            }
        } else {
            if (!chunks.count(raycast_chunk_pos)) {
                break;
            }
            current_chunk_pos = raycast_chunk_pos;
            current_chunk = chunks.at(raycast_chunk_pos);
        }

        math::vector3s32 raycast_block_pos = floor_float_position(raycast_pos);
        raycast_block_pos.x = math::mod(raycast_block_pos.x, chunk::SIZE);
        raycast_block_pos.y = math::mod(raycast_block_pos.y, chunk::SIZE);
        raycast_block_pos.z = math::mod(raycast_block_pos.z, chunk::SIZE);
        auto index = get_index_from_position(raycast_block_pos);
        auto& block = current_chunk->get().blocks[index];

        if (block.tp != block::type::AIR) {
            return raycast{ .ch_pos = *current_chunk_pos, .ch = *current_chunk, .bl_pos = raycast_block_pos, .bl = block };
        }

        raycast_pos += dir_vec;
    }
    return {};
}

void game::update_from_input(
    f32 cam_movement_speed,
    f32 cam_rotation_speed,
    u16 v_width,
    u16 v_height,
    camera& cam,
    cursor& cursor,
    std::optional<raycast>& raycast
) {
    input::scan_pads();
    u32 buttons_held = input::get_buttons_held(0);
    u32 buttons_down = input::get_buttons_down(0);
    if (buttons_down & WPAD_BUTTON_HOME) { std::exit(0); }

    auto joystick_input_vector = input::get_joystick_input_vector();
    auto plus_minus_input_scalar = input::get_plus_minus_input_scalar(buttons_held);

    if (math::is_non_zero(joystick_input_vector) || plus_minus_input_scalar != 0) {
        if (std::abs(joystick_input_vector.x) < 6.0f) {
            joystick_input_vector.x = 0.0f;
        }
        if (std::abs(joystick_input_vector.y) < 6.0f) {
            joystick_input_vector.y = 0.0f;
        }
        glm::vec3 input_vector = { joystick_input_vector.y / 96.0f, plus_minus_input_scalar, joystick_input_vector.x / 96.0f };
        move_camera(cam, input_vector, cam_movement_speed);
        
        cam.update_view = true;
    }

    auto pad_input_vector = input::get_dpad_input_vector(buttons_held);

    if (math::is_non_zero(pad_input_vector)) {
        math::normalize(pad_input_vector);
        pad_input_vector *= cam_rotation_speed;

        rotate_camera(cam, pad_input_vector, cam_rotation_speed);
        
        cam.update_view = true;
        cam.update_look = true;
    }

    auto pointer_pos = input::get_pointer_position();
    if (pointer_pos.has_value()) {
        cursor.tf.set_position(pointer_pos->x, pointer_pos->y);
    } else {
        cursor.tf.set_position((v_width / 2) - 24.f, (v_height / 2) - 24.f);
    }

    if (raycast.has_value() && buttons_down & WPAD_BUTTON_A) {
        raycast->bl = { .tp = block::type::AIR };
        raycast->ch.modified = true;
        add_chunk_mesh_update(raycast->ch, raycast->bl_pos);
    }
}