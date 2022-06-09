#include "game.hpp"

using namespace game;

void game::update_view(const camera& cam, math::matrix view) {
    auto look_at = cam.position + cam.rotation.look();
    guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
}

void game::move_camera_from_input_vector(camera& cam, const math::vector3f& input_vector, f32 move_speed) {
    math::matrix3f cam_rot = {
        cam.rotation.look(),
        { 0, cam.rotation.up().y, 0 },
        { cam.rotation.right().x, 0, cam.rotation.right().z }
    };
    cam_rot.normalize();

    math::vector3f move_vector = cam_rot * input_vector;

    move_vector *= move_speed;

    cam.position += move_vector;
}

void game::rotate_camera(camera& cam, f32 x, f32 y) {
    // Shift the look vector right by the x amount
    cam.rotation.look() += cam.rotation.right() * x;
    cam.rotation.look().normalize();

    // Keep the right vector orthogonal to the look vector
    auto inverse_a = cam.rotation.look() * -1.f;
    cam.rotation.right() += inverse_a * x;
    cam.rotation.right().normalize();

    // Shift the up vector up by the y amount
    cam.rotation.look() += cam.rotation.up() * y;
    cam.rotation.look().normalize();

    // Keep the up vector orthogonal to the look vector
    auto inverse_b = cam.rotation.look() * -1.f;
    cam.rotation.up() += inverse_b * y;
    cam.rotation.up().normalize();
}