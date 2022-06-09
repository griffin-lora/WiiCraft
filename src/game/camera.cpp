#include "game.hpp"

using namespace game;

void game::update_view(const camera& cam, math::matrix view) {
    auto look_at = cam.position + math::look(cam.rotation);
    guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
}

void game::move_camera_from_input_vector(camera& cam, const glm::vec3& input_vector, f32 move_speed) {
    glm::mat3x3 cam_rot = {
        math::look(cam.rotation),
        { 0, math::up(cam.rotation).y, 0 },
        { math::right(cam.rotation).x, 0, math::right(cam.rotation).z }
    };
    glm::normalize(cam_rot[0]);
    glm::normalize(cam_rot[1]);
    glm::normalize(cam_rot[2]);

    glm::vec3 move_vector = cam_rot * input_vector;

    move_vector *= move_speed;

    cam.position += move_vector;
}

void game::rotate_camera(camera& cam, f32 x, f32 y) {
    // Shift the look vector right by the x amount
    math::look(cam.rotation) += math::right(cam.rotation) * x;
    glm::normalize(math::look(cam.rotation));

    // Keep the right vector orthogonal to the look vector
    auto inverse_a = math::look(cam.rotation) * -1.f;
    math::right(cam.rotation) += inverse_a * x;
    glm::normalize(math::right(cam.rotation));

    // Shift the up vector up by the y amount
    math::look(cam.rotation) += math::up(cam.rotation) * y;
    glm::normalize(math::look(cam.rotation));

    // Keep the up vector orthogonal to the look vector
    auto inverse_b = math::look(cam.rotation) * -1.f;
    math::up(cam.rotation) += inverse_b * y;
    glm::normalize(math::up(cam.rotation));
}