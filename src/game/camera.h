#pragma once
#include "game_math.h"
#include "chrono.h"
#include <ogc/gu.h>
#include <cglm/struct/mat4.h>

extern vec3s cam_position;
extern vec3s cam_up;
extern vec3s cam_forward;
extern f32 cam_yaw;
extern f32 cam_pitch;

extern f32 fov;
extern f32 aspect;
extern f32 near_clipping_plane_distance;
extern f32 far_clipping_plane_distance;

extern us_t fov_tween_start;

void camera_update(f32 delta, u32 buttons_held);
void camera_update_visuals(us_t now, mat4s* projection, mat4s* view);