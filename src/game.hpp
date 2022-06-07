#pragma once
#include "math.hpp"
#include <gccore.h>
#include <vector>

namespace game {
    struct camera {
        math::vector3f position;
        math::vector3f up;
        math::vector3f look;
        f32 fov;
        f32 aspect;
        f32 near_clipping_plane_distance;
        f32 far_clipping_plane_distance;
    };

    struct camera_update_params {
        bool update_view = false;
	    bool update_perspective = false;
    };

    inline void update_view(const camera& cam, math::matrix view) {
        guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&cam.look);
    }
    inline void update_perspective(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    inline void set_look_vector(camera& cam, const math::vector3f& look) {
        cam.look = cam.position + look;
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
                using it = std::vector<chunk::mesh::vertex>::iterator;
                using const_it = std::vector<chunk::mesh::vertex>::const_iterator;
            };
            std::vector<vertex> vertices;
            vertex::it vertices_end;
        };
    };

    std::size_t get_center_vertex_count(block::type type);
    std::size_t get_any_face_vertex_count(block::type type);

    template<block::face face>
    constexpr std::size_t get_face_vertex_count(block::type type) {
        if constexpr (face == block::face::center) {
            return get_center_vertex_count(type);
        } else {
            return get_any_face_vertex_count(type);
        }
    }

    void add_center_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_front_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_back_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_left_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_right_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_top_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);
    void add_bottom_vertices(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type);

    template<block::face face>
    constexpr void add_face_vertices_at_mut_it(math::vector3u8 local_position, chunk::mesh::vertex::it& it, block::type type) {
        if constexpr (face == block::face::center) {
            add_center_vertices(local_position, it, type);
        } else if constexpr (face == block::face::front) {
            add_front_vertices(local_position, it, type);
        } else if constexpr (face == block::face::back) {
            add_back_vertices(local_position, it, type);
        } else if constexpr (face == block::face::left) {
            add_left_vertices(local_position, it, type);
        } else if constexpr (face == block::face::right) {
            add_right_vertices(local_position, it, type);
        } else if constexpr (face == block::face::top) {
            add_top_vertices(local_position, it, type);
        } else if constexpr (face == block::face::bottom) {
            add_bottom_vertices(local_position, it, type);
        }
    }
    template<block::face face>
    constexpr chunk::mesh::vertex::it add_face_vertices_at(math::vector3u8 local_position, chunk::mesh::vertex::it it, block::type type) {
        add_face_vertices_at_mut_it<face>(local_position, it, type);
        return it;
    }

    void draw_chunk(const chunk& chunk);
    void draw_chunk_mesh(chunk::mesh::vertex::const_it begin, chunk::mesh::vertex::const_it end);
}