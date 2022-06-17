#include "camera.hpp"
#include <algorithm>

using namespace game;

void game::update_view(const camera& cam, math::matrix view) {
    auto look_at = cam.position + cam.look;
    guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
}

void game::move_camera(camera& cam, const glm::vec3& input_vector, f32 move_speed) {
    glm::mat3 move = {
        cam.look,
        { 0, 1, 0 },
        { -cam.look.z, 0, cam.look.x }
    };
    math::normalize(move[0]);
    math::normalize(move[1]);
    math::normalize(move[2]);

    glm::vec3 move_vector = move * input_vector;

    move_vector *= move_speed;

    cam.position += move_vector;
}

void game::rotate_camera(camera& cam, const glm::vec2& input_vector, f32 rotate_speed) {
    cam.yaw = cam.yaw - (input_vector.x * rotate_speed);
    cam.pitch = cam.pitch + (input_vector.y * rotate_speed);
    cam.pitch = std::clamp(cam.pitch, glm::radians(-89.9f), glm::radians(89.9f));
}

void game::update_look(camera& cam) {
    f32 xz_length = cosf(cam.pitch);
    
    cam.look = {xz_length * cosf(cam.yaw), sinf(cam.pitch), xz_length * sinf(-cam.yaw)};
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