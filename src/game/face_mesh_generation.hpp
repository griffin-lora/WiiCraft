#pragma once
#include "chunk_core.hpp"

namespace game {
    std::size_t get_center_vertex_count(block::type type);
    std::size_t get_any_face_vertex_count(block::type type);

    template<block::face face>
    constexpr std::size_t get_face_vertex_count(block::type type) {
        return call_face_func_for<face, std::size_t>(
            get_any_face_vertex_count,
            get_any_face_vertex_count,
            get_any_face_vertex_count,
            get_any_face_vertex_count,
            get_any_face_vertex_count,
            get_any_face_vertex_count,
            get_center_vertex_count,
            type
        );
    }

    using vertex_it = ext::data_array<chunk::mesh::vertex>::iterator;

    void add_front_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_back_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_top_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_bottom_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_right_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_left_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);
    void add_center_vertices(vertex_it& it, math::vector3u8 local_position, block::type type);

    template<block::face face>
    constexpr void add_face_vertices(vertex_it& it, math::vector3u8 local_position, block::type type) {
        call_face_func_for<face, void>(
            add_front_vertices,
            add_back_vertices,
            add_top_vertices,
            add_bottom_vertices,
            add_right_vertices,
            add_left_vertices,
            add_center_vertices,
            it, local_position, type
        );
    }
};