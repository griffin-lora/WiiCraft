#pragma once
#include "math.hpp"
#include "gfx.hpp"
#include "ext/data_array.hpp"
#include <vector>

namespace game {
    struct camera {
        math::vector3f position;
        math::vector3f up;
        math::matrix3f rotation;
        f32 fov;
        f32 aspect;
        f32 near_clipping_plane_distance;
        f32 far_clipping_plane_distance;
    };

    struct camera_update_params {
        bool update_view = false;
	    bool update_perspective = false;
    };

    void update_view(const camera& cam, math::matrix view);
    void move_camera_from_input_vector(camera& cam, const math::vector3f& input_vector, f32 move_speed);
    void rotate_camera(camera& cam, f32 x, f32 y);

    inline void update_perspective(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    struct block {
        enum class face : u8 {
            CENTER,
            FRONT,
            BACK,
            LEFT,
            RIGHT,
            TOP,
            BOTTOM
        };
        enum class type : u8 {
            AIR,
            DEBUG,
            GRASS
        };
        type tp;
    };

    struct chunk {
        static constexpr s32 SIZE = 32;
        static constexpr u32 CHUNK_SIZE = SIZE * SIZE * SIZE;
        
        struct mesh {
            struct vertex {
                math::vector3u8 local_position;
                math::vector2u8 uv_position;
            };
            ext::data_array<vertex> vertices;
        };

        mesh ms;
        ext::data_array<block> blocks = { CHUNK_SIZE };

        const math::vector3s position;
        math::matrix model;
        math::matrix model_view;
    };

    inline void update_model_view(chunk& chunk, math::matrix view) {
        guMtxConcat(view, chunk.model, chunk.model_view);
    }

    void init(chunk& chunk, math::matrix view);

    std::size_t get_center_vertex_count(block::type type);
    std::size_t get_any_face_vertex_count(block::type type);

    template<block::face face>
    constexpr std::size_t get_face_vertex_count(block::type type) {
        if constexpr (face == block::face::CENTER) {
            return get_center_vertex_count(type);
        } else {
            return get_any_face_vertex_count(type);
        }
    }

    using vertex_it = ext::data_array<chunk::mesh::vertex>::iterator;

    void add_center_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_front_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_back_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_left_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_right_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_top_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);
    void add_bottom_vertices(math::vector3u8 local_position, vertex_it& it, block::type type);

    template<block::face face>
    constexpr void add_face_vertices_at_mut_it(math::vector3u8 local_position, vertex_it& it, block::type type) {
        if constexpr (face == block::face::CENTER) {
            add_center_vertices(local_position, it, type);
        } else if constexpr (face == block::face::FRONT) {
            add_front_vertices(local_position, it, type);
        } else if constexpr (face == block::face::BACK) {
            add_back_vertices(local_position, it, type);
        } else if constexpr (face == block::face::LEFT) {
            add_left_vertices(local_position, it, type);
        } else if constexpr (face == block::face::RIGHT) {
            add_right_vertices(local_position, it, type);
        } else if constexpr (face == block::face::TOP) {
            add_top_vertices(local_position, it, type);
        } else if constexpr (face == block::face::BOTTOM) {
            add_bottom_vertices(local_position, it, type);
        }
    }
    template<block::face face>
    constexpr auto add_face_vertices_at(math::vector3u8 local_position, vertex_it it, block::type type) {
        add_face_vertices_at_mut_it<face>(local_position, it, type);
        return it;
    }

    void draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices);
    void draw_chunk(chunk& chunk);
}