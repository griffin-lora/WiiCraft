#pragma once
#include "game_math.h"
#include "chrono.h"
#include <ogc/gu.h>

#define BASE_FOV 90.0f
#define SPRINT_FOV (BASE_FOV + 10.0f)
#define FOV_TWEEN_TIME 150000

extern vec3s cam_position;
extern vec3s cam_up;
extern vec3s cam_look;
extern f32 cam_yaw;
extern f32 cam_pitch;

extern f32 fov;
extern f32 aspect;
extern f32 near_clipping_plane_distance;
extern f32 far_clipping_plane_distance;

extern us_t fov_tween_start;

void camera_update(f32 delta, f32 cam_rotation_speed, u32 buttons_held);
void camera_update_visuals(us_t now, Mtx view, Mtx44 perspective);