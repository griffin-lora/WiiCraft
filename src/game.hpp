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

    inline void update_view(const camera& cam, math::matrix view) {
        auto look_at = cam.position + cam.rotation.look();
        guLookAt(view, (guVector*)&cam.position, (guVector*)&cam.up, (guVector*)&look_at);
    }
    inline void update_perspective(const camera& cam, math::matrix44 perspective) {
        guPerspective(perspective, cam.fov, cam.aspect, cam.near_clipping_plane_distance, cam.far_clipping_plane_distance);
    }

    inline void move_camera_from_input_vector(camera& cam, math::vector3f input_vector, f32 move_speed) {
        math::matrix3f cam_rot = {
            cam.rotation.look(),
            { 0, cam.rotation.up().y, 0 },
            { cam.rotation.right().x, 0, cam.rotation.right().z }
        };
        cam_rot.normalize();

        math::vector3f move_vector = cam_rot * input_vector;

        move_vector *= move_speed;

        cam.position += move_vector;
    }

    void rotate_camera(camera& cam, f32 x, f32 y);

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

    inline void init(chunk& chunk, math::matrix view) {
        guMtxIdentity(chunk.model);
        guMtxTransApply(chunk.model, chunk.model, chunk.position.x * chunk::SIZE, chunk.position.y * chunk::SIZE, chunk.position.z * chunk::SIZE);
        update_model_view(chunk, view);
    }

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

    inline void draw_chunk_mesh_vertices(const ext::data_array<chunk::mesh::vertex>& vertices) {
        gfx::draw_quads(vertices.size(), [&vertices]() {
            for (auto& v : vertices) {
                gfx::draw_vertex((f32)v.local_position.x, (f32)v.local_position.y, (f32)v.local_position.z, ((f32)v.uv_position.x)/16.f, ((f32)v.uv_position.y)/16.f);
            }
        });
    }
    inline void draw_chunk(chunk& chunk) {
		// load the modelview matrix into matrix memory
		gfx::set_position_matrix_into_index(chunk.model_view, GX_PNMTX3);
		
		gfx::set_position_matrix_from_index(GX_PNMTX3);

		game::draw_chunk_mesh_vertices(chunk.ms.vertices);
    }
}