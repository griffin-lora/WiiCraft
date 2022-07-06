#include "character.hpp"
#include "block_core.hpp"
#include "block_core.inl"
#include "block_raycast.hpp"
#include "block_raycast.inl"
#include "chunk_core.hpp"
#include "common.hpp"
#include "input.hpp"
#include "logic.hpp"
#include "dbg.hpp"

#include <cstdio>

using namespace game;

constexpr f32 movement_accel = 40.0f;
constexpr f32 max_movement_speed = 9.0f;
constexpr f32 movement_decel_factor = 0.005f;
constexpr f32 gravity = 0.6f;
constexpr f32 jump_velocity = 10.0f;

void character::handle_input(const camera& cam, u32 buttons_down) {
    #ifndef PC_PORT
    auto joystick_input_vector = input::get_joystick_input_vector();
    #else
    glm::vec2 joystick_input_vector = { 96.0f, 0.0f };
    #endif

    if ((buttons_down & WPAD_BUTTON_1) && grounded) {
        velocity.y = jump_velocity;
    }

    if (math::is_non_zero(joystick_input_vector)) {
        if (std::abs(joystick_input_vector.x) < 6.0f) {
            joystick_input_vector.x = 0.0f;
        }
        if (std::abs(joystick_input_vector.y) < 6.0f) {
            joystick_input_vector.y = 0.0f;
        }
        if (joystick_input_vector.x == 0.0f && joystick_input_vector.y == 0.0f) {
            apply_no_movement();
        } else {
            glm::vec3 input_vector = { joystick_input_vector.y / 96.0f, 0.0f, joystick_input_vector.x / 96.0f };
            apply_movement(cam, input_vector);
        }
    } else {
        apply_no_movement();
    }
}

void character::apply_movement(const camera& cam, glm::vec3 input_vector) {
    glm::mat3 movement_matrix = {
        { cam.look.x, 0, cam.look.z },
        { 0, 1, 0 },
        { -cam.look.z, 0, cam.look.x }
    };
    math::normalize(movement_matrix[0]);
    math::normalize(movement_matrix[1]);
    math::normalize(movement_matrix[2]);

    input_vector = movement_matrix * input_vector;

    auto accel_vector = input_vector * movement_accel;

    glm::vec3 move_vector = { velocity.x, 0.0f, velocity.z };
    move_vector += accel_vector * (1/60.0f);

    if (math::is_non_zero(move_vector) & math::is_non_zero(input_vector)) {
        auto len = glm::length(move_vector);
        auto max = max_movement_speed * glm::length(input_vector);
        if (len > max) {
            move_vector /= len;
            move_vector *= max;
        }
    }
    velocity = { move_vector.x, velocity.y, move_vector.z };
}

void character::apply_no_movement() {
    glm::vec3 move_vector = { velocity.x, 0.0f, velocity.z };
    if (math::is_non_zero(move_vector)) {
        move_vector *= movement_decel_factor * 60.0f;
        velocity = { move_vector.x, velocity.y, move_vector.z };
    }
}

static constexpr glm::vec3 half_size = { 0.35f, 1.0f, 0.35f };
static constexpr glm::vec3 full_size = half_size * 2.0f;

void character::apply_physics(chunk::map& chunks) {
    auto direction = velocity * (1.0f/60.0f);

    // replace this with specific offsets
    auto begin = position - glm::vec3{ 4.0f, 4.0f, 4.0f };
    auto end = position + glm::vec3{ 4.0f, 4.0f, 4.0f };

    auto raycast = game::get_block_raycast(chunks, position, direction, begin, end, []<typename Bf>(game::bl_st st) {
        return Bf::get_collision_boxes(st);
    }, [](auto& box) {
        box.lesser_corner -= half_size;
        box.greater_corner += half_size;
    });

    bool floor_collision = false;

    if (raycast.has_value() && math::is_non_zero(raycast->box_raycast.normal) && math::is_non_zero(velocity)) {
        if (raycast->box_raycast.normal.y == 1.0f) {
            floor_collision = true;
        }
        glm::vec3 absolute_normal = glm::abs(raycast->box_raycast.normal);
        glm::vec3 inverse_normal = { absolute_normal.x != 0 ? 0 : 1, absolute_normal.y != 0 ? 0 : 1, absolute_normal.z != 0 ? 0 : 1 };

        position = (raycast->box_raycast.intersection_position * absolute_normal) + (position * inverse_normal);
        velocity *= inverse_normal;
    }

    if (!floor_collision) {
        velocity.y -= gravity;
        grounded = false;
    }
}

void character::apply_velocity() {
    position += velocity * (1.0f/60.0f);
}

void character::update_camera(camera& cam) const {
    cam.position = { position.x, position.y + 0.5f, position.z };

    cam.update_view = true;
}