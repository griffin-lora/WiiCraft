#pragma once
#include "math.hpp"
#include "block_util.hpp"
#include "chunk.hpp"

namespace game {
    struct draw_positions {
        math::vector3u8 block_draw_pos;
        math::vector2u8 uv_draw_pos;
    };

    using const_quad_verts_ref = const chunk::quad&;

    #define ADD_FACE_TEMPLATE template<typename M, void (M::*Av)(game::const_quad_verts_ref)>

    ADD_FACE_TEMPLATE void add_flat_front_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);
    ADD_FACE_TEMPLATE void add_flat_back_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);
    ADD_FACE_TEMPLATE void add_flat_top_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);
    ADD_FACE_TEMPLATE void add_flat_bottom_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);
    ADD_FACE_TEMPLATE void add_flat_right_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);
    ADD_FACE_TEMPLATE void add_flat_left_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);

    ADD_FACE_TEMPLATE void add_foliage_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions);

    template<block::face face, typename M, void (M::*Av)(const_quad_verts_ref)>
    constexpr void add_flat_face_vertices(M& ms_st, const draw_positions& d_positions, const draw_positions& offset_d_positions) {
        call_face_func_for<face, void>(
            add_flat_front_vertices<M, Av>,
            add_flat_back_vertices<M, Av>,
            add_flat_top_vertices<M, Av>,
            add_flat_bottom_vertices<M, Av>,
            add_flat_right_vertices<M, Av>,
            add_flat_left_vertices<M, Av>,
            ms_st, d_positions, offset_d_positions
        );
    }

    template<block::face face, typename Bf, typename M, void (M::*Av)(const_quad_verts_ref)>
    constexpr void add_flat_face_vertices_from_block_position(M& ms_st, math::vector3u8 block_pos, bl_st st) {
        // Get information about where the vertices will be positioned
        draw_positions d_positions = {
            .block_draw_pos = block_pos * block_draw_size,
            .uv_draw_pos = Bf::template get_uv_position<face>(st) * block_draw_size
        };
        d_positions = Bf::get_draw_positions(d_positions, st);
        draw_positions offset_d_positions = Bf::template get_offset_draw_positions<face>(d_positions, st);
        add_flat_face_vertices<face, M, Av>(
            ms_st,
            d_positions,
            offset_d_positions
        );
    }
}