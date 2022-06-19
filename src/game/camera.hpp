#pragma once
#include "math.hpp"

namespace game {
    struct camera {
        glm::vec3 position;
        glm::vec3 up;
        glm::vec3 look;
        f32 yaw = 0.0f;
        f32 pitch = 0.0f;
        f32 fov;
        f32 aspect;
        f32 near_clipping_plane_distance;
        f32 far_clipping_plane_distance;

        bool update_view = false;
        bool update_look = false;
	    bool update_perspective = false;
    };

    void update_view(const camera& cam, math::matrix view);
    void update_look(camera& cam);

    void move_camera(camera& cam, const glm::vec3& input_vector, f32 move_speed);
    void rotate_camera(camera& cam, const glm::vec2& input_vector, f32 rotate_speed);
    
    template<typename T>
    T get_camera_look_axis_aligned_vector(const camera& cam) {
        if (std::abs(cam.look.x) > std::abs(cam.look.y) && std::abs(cam.look.x) > std::abs(cam.look.z)) {
            return { cam.look.x, 0, 0 };
        } else if (std::abs(cam.look.y) > std::abs(cam.look.x) && std::abs(cam.look.y) > std::abs(cam.look.z)) {
            return { 0, cam.look.y, 0 };
        } else {
            return { 0, 0, cam.look.z };
        }
    }

    inline void update_perspective(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    void update_needed(math::matrix view, math::matrix44 perspective, camera& cam);
    void reset_update_params(camera& cam);
};