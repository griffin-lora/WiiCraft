#include "character.hpp"
#include "block_core.hpp"
#include "block_core.inl"
#include "chunk_core.hpp"
#include "common.hpp"
#include "input.hpp"
#include "logic.hpp"

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
    glm::vec2 joystick_input_vector = { 96.0f, 48.0f };
    #endif

    if (buttons_down & WPAD_BUTTON_1 && grounded) {
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

template<typename Fc, typename Fn>
void character::apply_collision(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& dir, Fc collision_func, Fn no_collision_func) {
    auto raycast = get_raycast(origin, dir, 64, chunks);
    if (raycast.has_value()) {
        const auto world_block_pos = floor_float_position<glm::vec3>(raycast->pos);
        const auto box = get_box_that_collides_with_world_position(raycast->pos, raycast->bl, world_block_pos);
        if (box.has_value()) {
            collision_func(world_block_pos, *box);
        } else {
            no_collision_func();
        }
    } else {
        no_collision_func();
    }
}

void character::apply_physics(chunk::map& chunks) {
    auto block = get_block_from_world_position(chunks, position);
    if (block.has_value() && block->get().tp == block::type::AIR) {
        math::box character_box = {
            .lesser_corner = { -0.35f, -1.0f, -0.35f },
            .greater_corner = { 0.35f, 1.0f, 0.35f },
        };
        character_box.lesser_corner += position;
        character_box.greater_corner += position;

        auto floored_lesser_corner = floor_float_position<glm::vec3>(character_box.lesser_corner);
        auto floored_greater_corner = floor_float_position<glm::vec3>(character_box.greater_corner);
        
        for (f32 x = floored_lesser_corner.x; x <= floored_greater_corner.x; x++) {
            for (f32 y = floored_lesser_corner.y; y <= floored_greater_corner.y; y++) {
                for (f32 z = floored_lesser_corner.z; z <= floored_greater_corner.z; z++) {
                    glm::vec3 world_block_pos = { x, y, z };
                    auto block = get_block_from_world_position(chunks, world_block_pos);
                    if (block.has_value()) {
                        auto collided_block_boxes = get_block_boxes_that_collides_with_world_box(character_box, *block, world_block_pos);
                        if (collided_block_boxes.size() > 0) {
                            velocity.y = 0.0f;
                            return;
                        }
                    }
                }
            }
        }
        velocity.y -= gravity;
    } else {
        velocity.y = 0.0f;
    }
}

void character::apply_velocity() {
    position += velocity * (1.0f/60.0f);
}

void character::update_camera(camera& cam) const {
    cam.position = { position.x, position.y + 0.5f, position.z };

    cam.update_view = true;
}