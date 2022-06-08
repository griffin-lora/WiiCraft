#include "game.hpp"

using namespace game;

void game::rotate_camera(camera& cam, f32 x, f32 y) {
    cam.rotation.look() += cam.rotation.right() * x;
    cam.rotation.look().normalize();
    auto inverse_a = cam.rotation.look() * -1.f;
    cam.rotation.right() += inverse_a * x;
    cam.rotation.right().normalize();

    cam.rotation.look() += cam.rotation.up() * y;
    cam.rotation.look().normalize();
    auto inverse_b = cam.rotation.look() * -1.f;
    cam.rotation.up() += inverse_b * y;
    cam.rotation.up().normalize();
}