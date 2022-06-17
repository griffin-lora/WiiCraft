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

    /** Vf stands for a vertex function type
     * Vf::call(u8 x, u8 y, u8 z, u8 u, u8 v)
     */

    template<typename Vf> void add_front_vertices(math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_back_vertices(math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_top_vertices( math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_bottom_vertices(math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_right_vertices(math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_left_vertices(math::vector3u8 local_position, block::type type);
    template<typename Vf> void add_center_vertices(math::vector3u8 local_position, block::type type);

    template<block::face face, typename Vf>
    constexpr void add_face_vertices(math::vector3u8 local_position, block::type type) {
        call_face_func_for<face, void>(
            add_front_vertices<Vf>,
            add_back_vertices<Vf>,
            add_top_vertices<Vf>,
            add_bottom_vertices<Vf>,
            add_right_vertices<Vf>,
            add_left_vertices<Vf>,
            add_center_vertices<Vf>,
            local_position, type
        );
    }
};