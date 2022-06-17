#include "input.hpp"
#include "game/chunk_core.hpp"

using namespace input;

void input::init(u32 width, u32 height) {
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, width, height);
}

static glm::vec2 get_dpad_input_vector(u32 buttons_held) {
    glm::vec2 pad_input_vector = {0.0f, 0.0f};
    if (buttons_held & WPAD_BUTTON_RIGHT) {
        pad_input_vector.x += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_LEFT) {
        pad_input_vector.x -= 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_UP) {
        pad_input_vector.y += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_DOWN) {
        pad_input_vector.y -= 1.0f;
    }
    return pad_input_vector;
}

static glm::vec2 get_pointer_input_vector(state& s, u32 buttons_held) {
    ir_t pointer;
    WPAD_IR(0, &pointer);
    if (pointer.valid) {
        glm::vec2 pointer_pos = {pointer.sx, pointer.sy};
        if ((buttons_held & WPAD_BUTTON_A) && s.was_last_pointer_pos_valid && pointer_pos != s.last_pointer_pos) {
            glm::vec2 pointer_input_vector = pointer_pos - s.last_pointer_pos;
            s.was_last_pointer_pos_valid = true;
            s.last_pointer_pos = pointer_pos;
            return pointer_input_vector;
        }
        s.was_last_pointer_pos_valid = true;
        s.last_pointer_pos = pointer_pos;
    } else {
        s.was_last_pointer_pos_valid = false;
    }
    return {0.0f, 0.0f};
}

static glm::vec2 get_joystick_input_vector() {
    expansion_t exp;
    WPAD_Expansion(0, &exp);

    if (exp.type == WPAD_EXP_NUNCHUK) {
        return { exp.nunchuk.js.pos.x - exp.nunchuk.js.center.x, exp.nunchuk.js.pos.y - exp.nunchuk.js.center.y };
    }
    return { 0.0f, 0.0f };
}

static float get_plus_minus_input_scalar(u32 buttons_held) {
    float scalar = 0.0f;
    if (buttons_held & WPAD_BUTTON_PLUS) {
        scalar += 1.0f;
    }
    if (buttons_held & WPAD_BUTTON_MINUS) {
        scalar -= 1.0f;
    }
    return scalar;
}

void input::handle(f32 cam_movement_speed, f32 cam_rotation_speed, game::camera& cam, std::optional<game::raycast>& raycast) {
    WPAD_ScanPads();
    u32 buttons_held = get_buttons_held(0);
    u32 buttons_down = get_buttons_down(0);
    if (buttons_down & WPAD_BUTTON_HOME) { std::exit(0); }

    auto joystick_input_vector = get_joystick_input_vector();
    auto plus_minus_input_scalar = get_plus_minus_input_scalar(buttons_held);

    if (math::is_non_zero(joystick_input_vector) || plus_minus_input_scalar != 0) {
        if (std::abs(joystick_input_vector.x) < 6.0f) {
            joystick_input_vector.x = 0.0f;
        }
        if (std::abs(joystick_input_vector.y) < 6.0f) {
            joystick_input_vector.y = 0.0f;
        }
        glm::vec3 input_vector = { joystick_input_vector.y / 96.0f, plus_minus_input_scalar, joystick_input_vector.x / 96.0f };
        game::move_camera(cam, input_vector, cam_movement_speed);
        
        cam.update_view = true;
    }

    auto pad_input_vector = get_dpad_input_vector(buttons_held);

    if (math::is_non_zero(pad_input_vector)) {
        math::normalize(pad_input_vector);
        pad_input_vector *= cam_rotation_speed;

        game::rotate_camera(cam, pad_input_vector, cam_rotation_speed);
        
        cam.update_view = true;
        cam.update_look = true;
    }

    if (raycast.has_value() && buttons_down & WPAD_BUTTON_A) {
        raycast->bl = { .tp = game::block::type::AIR };
        raycast->ch.modified = true;
        game::add_chunk_mesh_update(raycast->ch, raycast->bl_pos);
    }
}