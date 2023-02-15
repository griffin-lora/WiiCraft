#include "camera.hpp"
#include "input.hpp"
#include "character.hpp"
#include <algorithm>
#include <ogc/gu.h>

using namespace game;

void game::update_camera(camera& cam, us_t now) {
    cam.position = (vec3s){ character_position.raw[0], character_position.raw[1] + 0.9f, character_position.raw[2] };

    cam.update_view = true;

    auto elapsed = now - fov_tween_start;

    if (elapsed <= FOV_TWEEN_TIME) {
        auto alpha = get_eased(elapsed / (f32)FOV_TWEEN_TIME);

        if (sprinting) {
            cam.fov = lerp(BASE_FOV, SPRINT_FOV, alpha);
        } else {
            cam.fov = lerp(SPRINT_FOV, BASE_FOV, alpha);
        }

        cam.update_perspective = true;
    }
}

void game::update_view(const camera& cam, Mtx view) {
    vec3s look_at = glms_vec3_add(cam.position, cam.look);
    guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
}

void game::rotate_camera(camera& cam, vec2s move_vector) {
    cam.yaw = cam.yaw - move_vector.raw[0];
    cam.pitch = cam.pitch + move_vector.raw[1];
    cam.pitch = std::clamp(cam.pitch, DegToRad(-89.9f), DegToRad(89.9f));
}

void game::update_look(camera& cam) {
    f32 xz_length = cosf(cam.pitch);
    
    cam.look = glms_vec3_normalize((vec3s){xz_length * cosf(cam.yaw), sinf(cam.pitch), xz_length * sinf(-cam.yaw)});
}

void game::update_needed(Mtx view, Mtx44 perspective, camera& cam) {
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
    vec2s pad_input_vector = get_dpad_input_vector(buttons_held);

    if (pad_input_vector.raw[0] != 0 || pad_input_vector.raw[1] != 0) {
        glm_vec2_normalize(pad_input_vector.raw);
        pad_input_vector = glms_vec2_scale(pad_input_vector, cam_rotation_speed * delta);

        rotate_camera(cam, pad_input_vector);
        
        cam.update_view = true;
        cam.update_look = true;
    }
}