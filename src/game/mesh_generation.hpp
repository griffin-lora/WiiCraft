#pragma once
#include "chunk.hpp"
#include "ext/data_array.hpp"
#include "gfx/display_list.hpp"

namespace game {
    struct standard_vertex_function {
        ext::data_array<chunk::quad>::iterator quad_it;
        std::size_t standard_quad_count = 0;
        std::size_t foliage_quad_count = 0;

        inline void add_standard(const chunk::quad::vertices& vertices) {
            standard_quad_count++;
            *quad_it++ = {
                .tp = chunk::quad::type::STANDARD,
                .verts = vertices
            };
        }

        inline void add_foliage(const chunk::quad::vertices& vertices) {
            foliage_quad_count++;
            *quad_it++ = {
                .tp = chunk::quad::type::FOLIAGE,
                .verts = vertices
            };
        }
    };

    template<typename Lf, typename Df>
    void write_into_display_lists(const ext::data_array<chunk::quad>& building_quads, const standard_vertex_function& vf,
        gfx::display_list& standard_disp_list,
        gfx::display_list& foliage_disp_list,
        Lf get_display_list_size, Df draw_vert
    );
}