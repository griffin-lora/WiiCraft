#include "game.hpp"

using namespace game;

void game::rotate_camera(camera& cam, f32 x, f32 y) {
    cam.rotation.vec1 += cam.rotation.vec3 * x;
    cam.rotation.vec1.normalize();
    auto inverse_a = cam.rotation.vec1 * -1.f;
    cam.rotation.vec3 += inverse_a * x;
    cam.rotation.vec3.normalize();

    cam.rotation.vec1 += cam.rotation.vec2 * y;
    cam.rotation.vec1.normalize();
    auto inverse_b = cam.rotation.vec1 * -1.f;
    cam.rotation.vec2 += inverse_b * y;
    cam.rotation.vec2.normalize();
}