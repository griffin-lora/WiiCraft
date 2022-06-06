#pragma once
#include "math.hpp"
#include <gccore.h>
#include <vector>

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

    struct block {
        enum class face {
            center,
            front,
            back,
            left,
            right,
            top,
            bottom
        };
        enum class type {
            AIR,
            GRASS
        };
        type tp;
    };

    struct chunk {
        struct mesh {
            struct vertex {
                math::vector3u8 local_position;
                math::vector2u8 uv_position;
            };
            std::vector<vertex> vertices;
        };
    };

    using vert_it = std::vector<chunk::mesh::vertex>::iterator;
    using const_vert_it = std::vector<chunk::mesh::vertex>::const_iterator;

    void add_face_vertices_at_mut_it(math::vector3u8 local_position, vert_it& it, block::type type, block::face face);
    inline vert_it add_face_vertices_at(math::vector3u8 local_position, vert_it it, block::type type, block::face face) {
        add_face_vertices_at_mut_it(local_position, it, type, face);
        return it;
    }
    void draw_chunk_mesh(const_vert_it begin, const_vert_it end);
}