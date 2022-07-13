#pragma once
#include "math.hpp"
#include "chunk.hpp"

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

    void rotate_camera(camera& cam, const glm::vec2& input_vector);

    inline void update_perspective(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    void update_needed(math::matrix view, math::matrix44 perspective, camera& cam);
    void reset_update_params(camera& cam);
}