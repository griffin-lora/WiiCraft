#include "logic.hpp"
#include "chunk_core.hpp"
#include "cursor.hpp"
#include "input.hpp"
#include "dbg.hpp"

using namespace game;

void game::update_from_input(
    f32 cam_rotation_speed,
    u16 v_width,
    u16 v_height,
    character& character,
    camera& cam,
    chunk::map& chunks,
    cursor& cursor,
    f32 delta,
    std::optional<block_raycast>& raycast
) {
    input::scan_pads();
    u32 buttons_held = input::get_buttons_held(0);
    u32 buttons_down = input::get_buttons_down(0);
    if (buttons_down & WPAD_BUTTON_HOME) { std::exit(0); }

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


    #ifndef PC_PORT
    expansion_t exp;
    if (input::scan_nunchuk(exp)) {
        const auto& nunchuk = exp.nunchuk;
        
        auto nunchuk_vector = input::get_nunchuk_vector(nunchuk);

        auto nunchuk_buttons_down = nunchuk.btns;

        character.handle_input(cam, delta, nunchuk_vector, nunchuk_buttons_down);
    }
    #else
    character.handle_input(cam, delta, { 20.0f, 50.0f }, 0);
    #endif

    auto pad_input_vector = input::get_dpad_input_vector(buttons_held);

    if (math::is_non_zero(pad_input_vector)) {
        math::normalize(pad_input_vector);
        pad_input_vector *= (cam_rotation_speed * delta);

        rotate_camera(cam, pad_input_vector);
        
        cam.update_view = true;
        cam.update_look = true;
    }

    // Removing this for the time being since I have not yet implemented the mechanic where the raycast is eminated from the pointer's position

    // auto pointer_pos = input::get_pointer_position();
    // if (pointer_pos.has_value()) {
    //     cursor.tf.set_position(pointer_pos->x, pointer_pos->y);
    // } else {
        cursor.tf.set_position((v_width / 2) - 24.f, (v_height / 2) - 24.f);
        cursor.tf.load(cursor::MAT);
    // }
}