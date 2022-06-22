#include "character.hpp"
#include "logic.hpp"
#include "input.hpp"

using namespace game;

void character::handle_input(f32 movement_speed, const camera& cam, u32 buttons_down) {
    auto joystick_input_vector = input::get_joystick_input_vector();

    if (math::is_non_zero(joystick_input_vector)) {
        if (std::abs(joystick_input_vector.x) < 6.0f) {
            joystick_input_vector.x = 0.0f;
        }
        if (std::abs(joystick_input_vector.y) < 6.0f) {
            joystick_input_vector.y = 0.0f;
        }
        glm::vec3 input_vector = { joystick_input_vector.y / 96.0f, 0.0f, joystick_input_vector.x / 96.0f };
        move(movement_speed, cam, input_vector);
    } else {
        // TODO: add cancel_movement function
        velocity = { 0.0f, velocity.y, 0.0f };
    }
}

void character::move(f32 movement_speed, const camera& cam, const glm::vec3& input_vector) {
    glm::mat3 movement_matrix = {
        { cam.look.x, 0, cam.look.z },
        { 0, 1, 0 },
        { -cam.look.z, 0, cam.look.x }
    };
    math::normalize(movement_matrix[0]);
    math::normalize(movement_matrix[1]);
    math::normalize(movement_matrix[2]);

    glm::vec3 move_vector = movement_matrix * input_vector;

    move_vector *= movement_speed;

    velocity = { move_vector.x, move_vector.y + velocity.y, move_vector.z };
}

void character::apply_physics(f32 gravity, chunk::map& chunks) {
    auto raycast = get_raycast({ position.x, position.y - 0.4f, position.z }, { 0.0f, -1.0f, 0.0f }, 64, chunks);
    if (raycast.has_value()) {
        position.y = (std::floor(raycast->pos.y)) + 2.0f;
        velocity.y = 0;
    } else {
        velocity.y -= gravity;
    }

    position += velocity * (1.0f/60.0f);
}

void character::update_camera(camera& cam) const {
    cam.position = { position.x, position.y + 1.0f, position.z };

    cam.update_view = true;
}