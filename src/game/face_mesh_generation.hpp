#pragma once
#include "math.hpp"
#include "block_util.hpp"
#include "chunk.hpp"

namespace game {
    #define ADD_FACE_TEMPLATE template<typename L, typename M>
    #define FACE_PARAMS M& ms_st, math::vector3u8 pos, math::vector3u8 offset_pos, math::vector2u8 uv, math::vector2u8 offset_uv
    #define SHORT_FACE_PARAMS M& ms_st, math::vector3u8 p, math::vector3u8 po, math::vector2u8 u, math::vector2u8 uo
    #define TINTED_FACE_PARAMS M& ms_st, math::vector3u8 pos, math::vector3u8 offset_pos, math::vector3u8 color, math::vector2u8 uv, math::vector2u8 offset_uv
    #define SHORT_TINTED_FACE_PARAMS M& ms_st, math::vector3u8 p, math::vector3u8 po, math::vector3u8 c, math::vector2u8 u, math::vector2u8 uo

    ADD_FACE_TEMPLATE void add_flat_standard_front_vertices(FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_standard_back_vertices(FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_standard_top_vertices(FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_standard_bottom_vertices(FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_standard_right_vertices(FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_standard_left_vertices(FACE_PARAMS);

    ADD_FACE_TEMPLATE void add_flat_tinted_front_vertices(TINTED_FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_tinted_back_vertices(TINTED_FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_tinted_top_vertices(TINTED_FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_tinted_bottom_vertices(TINTED_FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_tinted_right_vertices(TINTED_FACE_PARAMS);
    ADD_FACE_TEMPLATE void add_flat_tinted_left_vertices(TINTED_FACE_PARAMS);

    ADD_FACE_TEMPLATE void add_foliage_vertices(TINTED_FACE_PARAMS);

    template<block::face FACE, typename L, typename M>
    constexpr void add_flat_standard_face_vertices(FACE_PARAMS) {
        call_face_func_for<FACE, void>(
            add_flat_standard_front_vertices<L, M>,
            add_flat_standard_back_vertices<L, M>,
            add_flat_standard_top_vertices<L, M>,
            add_flat_standard_bottom_vertices<L, M>,
            add_flat_standard_right_vertices<L, M>,
            add_flat_standard_left_vertices<L, M>,
            ms_st, pos, offset_pos, uv, offset_uv
        );
    }

    template<block::face FACE, typename L, typename M>
    constexpr void add_flat_tinted_face_vertices(TINTED_FACE_PARAMS) {
        call_face_func_for<FACE, void>(
            add_flat_tinted_front_vertices<L, M>,
            add_flat_tinted_back_vertices<L, M>,
            add_flat_tinted_top_vertices<L, M>,
            add_flat_tinted_bottom_vertices<L, M>,
            add_flat_tinted_right_vertices<L, M>,
            add_flat_tinted_left_vertices<L, M>,
            ms_st, pos, offset_pos, color, uv, offset_uv
        );
    }
}