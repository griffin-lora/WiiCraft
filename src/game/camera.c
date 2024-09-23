#include "camera.h"
#include "input.h"
#include "character.h"
#include <cglm/struct/cam.h>
#include <cglm/struct/vec2.h>
#include <cglm/struct/vec3.h>
#include <cglm/struct/mat3.h>
#include <cglm/struct/affine.h>
#include <ogc/gu.h>
#include <math.h>

#define CAM_ROTATION_SPEED 1.80f

#define BASE_FOV 90.0f
#define SPRINT_FOV (BASE_FOV + 10.0f)
#define FOV_TWEEN_TIME 150000

vec3s cam_position = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
vec3s cam_up = { .x = 0.0f, .y = 1.0f, .z = 0.0f };
vec3s cam_forward = { .x = 0.0f, .y = 0.0f, .z = 1.0f };
f32 cam_yaw = 0.0f;
f32 cam_pitch = 0.0f;

f32 fov = BASE_FOV;
f32 aspect;
f32 near_clipping_plane_distance = 0.1f;
f32 far_clipping_plane_distance = 300.0f;

us_t fov_tween_start = 0;

void camera_update(f32 delta, u32 buttons_held) {
    // Rotate camera, update look and view

    vec2s pad_input_vector = get_dpad_input_vector(buttons_held);
    if (IS_NON_ZERO_VEC2(pad_input_vector)) {
        glm_vec2_normalize(pad_input_vector.raw);
        pad_input_vector = glms_vec2_scale(pad_input_vector, CAM_ROTATION_SPEED * delta);

        cam_yaw = cam_yaw - pad_input_vector.x;
        cam_pitch = cam_pitch + pad_input_vector.y;

        if (cam_pitch > DegToRad(89.9f)) {
            cam_pitch = DegToRad(89.9f);
        } else if (cam_pitch < DegToRad(-89.9f)) {
            cam_pitch = DegToRad(-89.9f);
        }
    }

    f32 xz_length = cosf(cam_pitch);
    
    cam_forward = glms_vec3_normalize((vec3s){ .x = xz_length * cosf(cam_yaw), .y = sinf(cam_pitch), .z = xz_length * sinf(-cam_yaw) });
}

void camera_update_visuals(us_t now, mat4s* projection, mat4s* view) {
    us_t elapsed = now - fov_tween_start;

    if (elapsed <= FOV_TWEEN_TIME) {
        f32 alpha = get_eased((f32) elapsed / (f32) FOV_TWEEN_TIME);

        if (sprinting) {
            fov = lerpf(BASE_FOV, SPRINT_FOV, alpha);
        } else {
            fov = lerpf(SPRINT_FOV, BASE_FOV, alpha);
        }
    }

    guPerspective(projection->raw, fov, aspect, near_clipping_plane_distance, far_clipping_plane_distance);

    cam_position = glms_vec3_add(character_position, (vec3s){ .x = 0, .y = 0.9f, .z = 0 });

    vec3s look_at = glms_vec3_add(cam_position, cam_forward);

    guLookAt(view->raw, (const guVector*) &cam_position, (const guVector*) &cam_up, (const guVector*) &look_at);
}