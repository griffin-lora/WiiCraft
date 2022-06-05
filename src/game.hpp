#pragma once
#include "math.hpp"
#include <gccore.h>

namespace game {
    struct camera {
        math::vector3 position;
        math::vector3 up;
        math::vector3 look;
        f32 fov;
        f32 aspect;
        f32 near_clipping_plane_distance;
        f32 far_clipping_plane_distance;
    };

    struct camera_update_params {
        bool update_view = false;
	    bool update_perspective = false;
    };

    inline void update_view_from_camera(const camera& cam, math::matrix view) {
        guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&cam.look);
    }
    inline void update_perspective_from_camera(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    enum class face {
        center,
        front,
        back,
        left,
        right,
        top,
        bottom
    };

    struct block {
        enum class type {
            AIR,
            GRASS
        };
        type tp;
    };
}