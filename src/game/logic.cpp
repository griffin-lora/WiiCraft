#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

// Yes I know I should be using DDA algorithm but I couldnt FUCKING GET IT TO WORK IN 3D
constexpr f32 raycast_step_scalar = 0.01f;

std::optional<raycast> game::get_raycast(const glm::vec3& origin, const glm::vec3& dir, u16 step_count, chunk::map& chunks) {
    glm::vec3 raycast_pos = origin;
    glm::vec3 dir_vec = dir * raycast_step_scalar;
    std::optional<math::vector3s32> current_chunk_pos = {};
    chunk::opt_ref current_chunk = {};
    for (u16 i = 0; i < step_count; i++) {
        auto raycast_chunk_pos = get_chunk_position_from_world_position(raycast_pos);

        if (current_chunk_pos.has_value()) {
            if (raycast_chunk_pos != current_chunk_pos) {
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

        auto raycast_block_pos = get_local_block_position(raycast_pos);
        auto index = get_index_from_position(raycast_block_pos);
        auto& block = current_chunk->get().blocks[index];

        if (does_world_position_collide_with_block(raycast_pos, block.tp, floor_float_position<glm::vec3>(raycast_pos))) {
            return raycast{
                .pos = raycast_pos,
                .ch_pos = *current_chunk_pos,
                .ch = *current_chunk,
                .bl_pos = raycast_block_pos,
                .bl = block
            };
        }

        raycast_pos += dir_vec;
    }
    return {};
}

static std::optional<raycast> get_backtracked_raycast(const camera& cam, chunk::map& chunks, const raycast& rc) {
    glm::vec3 dir_vec = cam.look * raycast_step_scalar;

    auto inp_raycast_world_block_pos = floor_float_position<math::vector3s32>(rc.pos);

    glm::vec3 pos = rc.pos;

    for (u8 i = 0; i < 256; i++) {
        auto world_block_pos = floor_float_position<math::vector3s32>(pos);

        if (world_block_pos != inp_raycast_world_block_pos) {
            auto back_chunk_pos = get_chunk_position_from_world_position(pos);
    
            if (chunks.count(back_chunk_pos)) {
                auto& chunk = chunks.at(back_chunk_pos);

                auto back_block_pos = get_local_block_position(pos);

                auto index = get_index_from_position(back_block_pos);
                auto& block = chunk.blocks[index];

                if (!does_world_position_collide_with_block(pos, block.tp, world_block_pos)) {
                    return raycast{
                        .pos = pos,
                        .ch_pos = back_chunk_pos,
                        .ch = chunk,
                        .bl_pos = back_block_pos,
                        .bl = block
                    };
                }
            }
            return {};
        }

        pos -= dir_vec;
    }

    
    return {};
}

void game::update_from_input(
    f32 cam_rotation_speed,
    u16 v_width,
    u16 v_height,
    character& character,
    camera& cam,
    chunk::map& chunks,
    cursor& cursor,
    std::optional<raycast>& raycast
) {
    input::scan_pads();
    u32 buttons_held = input::get_buttons_held(0);
    u32 buttons_down = input::get_buttons_down(0);
    if (buttons_down & WPAD_BUTTON_HOME) { std::exit(0); }

    if (raycast.has_value()) {
        if (buttons_down & WPAD_BUTTON_A) {
            raycast->bl = { .tp = block::type::AIR };
            raycast->ch.modified = true;
            add_important_chunk_mesh_update(raycast->ch, raycast->bl_pos);
        }
        if (buttons_down & WPAD_BUTTON_B) {
            auto backtracked_raycast = get_backtracked_raycast(cam, chunks, *raycast);
            if (backtracked_raycast.has_value()) {
                backtracked_raycast->bl = { .tp = block::type::STONE_SLAB };
                backtracked_raycast->ch.modified = true;
                add_important_chunk_mesh_update(backtracked_raycast->ch, backtracked_raycast->bl_pos);
            }
        }
    }

    character.handle_input(cam);

    auto pad_input_vector = input::get_dpad_input_vector(buttons_held);

    if (math::is_non_zero(pad_input_vector)) {
        math::normalize(pad_input_vector);
        pad_input_vector *= cam_rotation_speed;

        rotate_camera(cam, pad_input_vector, cam_rotation_speed);
        
        cam.update_view = true;
        cam.update_look = true;
    }

    // Removing this for the time being since I have not yet implemented the mechanic where the raycast is eminated from the pointer's position

    // auto pointer_pos = input::get_pointer_position();
    // if (pointer_pos.has_value()) {
    //     cursor.tf.set_position(pointer_pos->x, pointer_pos->y);
    // } else {
        cursor.tf.set_position((v_width / 2) - 24.f, (v_height / 2) - 24.f);
    // }
}