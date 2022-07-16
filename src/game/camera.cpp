#include "camera.hpp"
#include "input.hpp"
#include <algorithm>

using namespace game;

void game::update_view(const camera& cam, math::matrix view) {
    auto look_at = cam.position + cam.look;
    guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
}

void game::rotate_camera(camera& cam, const glm::vec2& move_vector) {
    cam.yaw = cam.yaw - move_vector.x;
    cam.pitch = cam.pitch + move_vector.y;
    cam.pitch = std::clamp(cam.pitch, glm::radians(-89.9f), glm::radians(89.9f));
}

void game::update_look(camera& cam) {
    f32 xz_length = cosf(cam.pitch);
    
    cam.look = {xz_length * cosf(cam.yaw), sinf(cam.pitch), xz_length * sinf(-cam.yaw)};
	math::normalize(cam.look);
}

void game::update_needed(math::matrix view, math::matrix44 perspective, camera& cam) {
    if (cam.update_look) {
        game::update_look(cam);
    }
    if (cam.update_view) {
        game::update_view(cam, view);
    }
    if (cam.update_perspective) {
        game::update_perspective(cam, perspective);
    }
}

void game::reset_update_params(camera& cam) {
    cam.update_view = false;
    cam.update_look = false;
    cam.update_perspective = false;
}

void game::update_camera_from_input(f32 cam_rotation_speed, camera& cam, f32 delta, u32 buttons_held) {
    auto pad_input_vector = input::get_dpad_input_vector(buttons_held);

    if (math::is_non_zero(pad_input_vector)) {
        math::normalize(pad_input_vector);
        pad_input_vector *= (cam_rotation_speed * delta);

        rotate_camera(cam, pad_input_vector);
        
        cam.update_view = true;
        cam.update_look = true;
    }
}