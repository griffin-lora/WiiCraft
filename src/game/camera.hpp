#pragma once
#include "game_math.hpp"
#include "chrono.hpp"
#include <ogc/gu.h>

extern us_t fov_tween_start;

#define BASE_FOV 90.0f
#define SPRINT_FOV (BASE_FOV + 10.0f)
#define FOV_TWEEN_TIME 150000

namespace game {
    struct camera {

        vec3s position;
        vec3s up;
        vec3s look;
        f32 yaw = 0.0f;
        f32 pitch = 0.0f;

        f32 fov = BASE_FOV;
        f32 aspect;
        f32 near_clipping_plane_distance;
        f32 far_clipping_plane_distance;

        bool update_view = false;
        bool update_look = false;
	    bool update_perspective = false;
    };

    void update_camera(camera& cam, us_t now);

    void update_view(const camera& cam, Mtx view);
    void update_look(camera& cam);

    void rotate_camera(camera& cam, vec2s input_vector);

    inline void update_perspective(const camera& cam, Mtx44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    void update_needed(Mtx view, Mtx44 perspective, camera& cam);
    void reset_update_params(camera& cam);

    void update_camera_from_input(f32 cam_rotation_speed, camera& cam, f32 delta, u32 buttons_held);
}