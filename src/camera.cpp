#include "game.hpp"

using namespace game;

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