#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "block_util.hpp"
#include <array>

namespace game {
    #define BF_MB static constexpr

    template<block::type TYPE>
    struct block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::transparent; }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M&, const F&, bl_st, math::vector3u8) {}

        template<typename M, typename F>
        BF_MB void add_general_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 0> get_selection_boxes(bl_st) { return {}; }
        BF_MB std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::invisible; }
    };

    template<typename BF>
    struct cube_block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::opaque_cube; }

        template<block::face FACE>
        BF_MB bool is_face_invisible_with_neighbor(bl_st st, const block& nb) {
            auto category = get_with_block_functionality<block::category>(nb.tp, [&nb]<typename NB_BF>() {
                return NB_BF::get_category((game::block::state)game::block::slab_state::bottom);
            });
            return
                category == block::category::opaque_cube ||
                (FACE == block::face::top && category == block::category::opaque_bottom_slab) ||
                (FACE == block::face::bottom && category == block::category::opaque_top_slab);
        }

        template<block::face FACE, typename M, typename F>
        BF_MB void add_face_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos, math::vector3u8 offset_pos) {
            const block* nb_block = get_face_neighbor_block.template operator()<FACE>();
            if (
                nb_block == nullptr ||
                is_face_invisible_with_neighbor<FACE>(st, *nb_block)
            ) {
                return;
            }
            math::vector2u8 uv = BF::template get_face_uv<FACE>(st/* TODO: add this param: , *nb_block */) * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
            add_flat_face_vertices<FACE>(ms_st, pos, offset_pos, uv, offset_uv);
        }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            call_func_on_each_face<void>(
                [&]<block::face FACE>() { add_face_vertices<FACE>(ms_st, get_face_neighbor_block, st, pos, offset_pos); }
            );
        }

        template<typename M, typename F>
        BF_MB void add_general_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 1> get_boxes() {
            return {
                math::box{
                    { 0.0f, 0.0f, 0.0f },
                    { 1.0f, 1.0f, 1.0f }
                }
            };
        }

        BF_MB auto get_selection_boxes(bl_st) { return get_boxes(); }
        BF_MB auto get_collision_boxes(bl_st) { return get_boxes(); }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::fully_opaque; }
    };

    template<typename BF>
    struct slab_block_functionality {
        using state = block::slab_state;

        BF_MB block::category get_category(bl_st st) {
            switch (st.slab) {
                default: return block::category::opaque_cube;
                case state::top: return block::category::opaque_top_slab;
                case state::bottom: return block::category::opaque_bottom_slab;
            }
        }

        template<block::face FACE>
        BF_MB bool is_face_invisible_with_neighbor(bl_st st, const block& nb) {
            auto category = get_with_block_functionality<block::category>(nb.tp, [&nb]<typename NB_BF>() {
                return NB_BF::get_category((game::block::state)game::block::slab_state::bottom);
            });
            if (category == block::category::opaque_cube) {
                return true;
            }
            switch (st.slab) {
                case state::bottom: return
                    FACE == block::face::bottom ?
                    category == block::category::opaque_top_slab :
                    category == block::category::opaque_bottom_slab;
                case state::top: return
                    FACE == block::face::top ?
                    category == block::category::opaque_bottom_slab :
                    category == block::category::opaque_top_slab;
                case state::both: return
                    (FACE == block::face::top && category == block::category::opaque_bottom_slab) ||
                    (FACE == block::face::bottom && category == block::category::opaque_top_slab);
            }
            return true;
        }

        template<block::face FACE, typename M, typename F>
        BF_MB void add_face_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos, math::vector3u8 offset_pos) {
            if (
                (FACE == block::face::top && st.slab == state::bottom) ||
                (FACE == block::face::bottom && st.slab == state::top)
            ) {
                return;
            }

            const block* nb_block = get_face_neighbor_block.template operator()<FACE>();
            if (
                nb_block == nullptr ||
                is_face_invisible_with_neighbor<FACE>(st, *nb_block)
            ) {
                return;
            }
            math::vector2u8 uv = BF::template get_face_uv<FACE>(st) * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, (
                FACE != block::face::top && FACE != block::face::bottom ?
                (st.slab == state::both ? block_draw_size : half_block_draw_size) :
                block_draw_size
            ) };
            add_flat_face_vertices<FACE>(ms_st, pos, offset_pos, uv, offset_uv);
        }

        template<typename M, typename F>
        static void __attribute__((noinline)) add_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            if (st.slab == state::top) {
                pos.y += half_block_draw_size;
            }
            auto offset_pos = pos + math::vector3u8{ block_draw_size, st.slab == state::both ? block_draw_size : half_block_draw_size, block_draw_size };

            call_func_on_each_face<void>(
                [&]<block::face FACE>() { add_face_vertices<FACE>(ms_st, get_face_neighbor_block, st, pos, offset_pos); }
            );
        }

        template<typename M, typename F>
        static void __attribute__((noinline)) add_general_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            if (st.slab != state::both) {
                pos *= block_draw_size;
                if (st.slab == state::top) {
                    pos.y += half_block_draw_size;
                }
                auto offset_pos = pos + math::vector3u8{ block_draw_size, half_block_draw_size, block_draw_size };

                if (st.slab == state::bottom) {
                    math::vector2u8 uv = BF::template get_face_uv<block::face::top>(st) * block_draw_size;
                    math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
                    add_flat_face_vertices<block::face::top>(ms_st, pos, offset_pos, uv, offset_uv);
                } else {
                    math::vector2u8 uv = BF::template get_face_uv<block::face::bottom>(st) * block_draw_size;
                    math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
                    add_flat_face_vertices<block::face::bottom>(ms_st, pos, offset_pos, uv, offset_uv);
                }
            }
        }

        BF_MB std::array<math::box, 1> get_boxes(bl_st st) {
            return {
                math::box{
                    { 0.0f, (st.slab == state::top ? 0.5f : 0.0f), 0.0f },
                    { 1.0f, (st.slab == state::bottom ? 0.5f : 1.0f), 1.0f }
                }
            };
        }

        BF_MB auto get_selection_boxes(bl_st st) { return get_boxes(st); }
        BF_MB auto get_collision_boxes(bl_st st) { return get_boxes(st); }

        BF_MB block_counting_type get_block_counting_type(bl_st st) { return (
            st.slab == state::both ?
            block_counting_type::fully_opaque :
            block_counting_type::partially_opaque
        ); }
    };
    
    template<typename BF>
    struct foliage_block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::transparent; }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M&, const F&, bl_st, math::vector3u8) {}

        template<typename M, typename F>
        BF_MB void add_general_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            math::vector2u8 uv = BF::get_uv(st) * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };

            add_foliage_vertices(ms_st, pos, offset_pos, uv, offset_uv);
        }

        BF_MB std::array<math::box, 1> get_selection_boxes(bl_st) {
            return {
                math::box{
                    { 0.2f, 0.0f, 0.2f },
                    { 0.8f, 0.8f, 0.8f }
                }
            };
        }
        BF_MB std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::partially_opaque; }
    };

    template<>
    struct block_functionality<block::type::debug> : public cube_block_functionality<block_functionality<block::type::debug>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<FACE, v2u8>(
                []() { return v2u8{0, 0}; },
                []() { return v2u8{1, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{4, 0}; },
                []() { return v2u8{5, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::grass> : public cube_block_functionality<block_functionality<block::type::grass>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<FACE, v2u8>(
                []() { return v2u8{4, 0}; },
                []() { return v2u8{4, 0}; },
                []() { return v2u8{0, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{4, 0}; },
                []() { return v2u8{4, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::dirt> : public cube_block_functionality<block_functionality<block::type::dirt>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::sand> : public cube_block_functionality<block_functionality<block::type::sand>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 6, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::stone> : public cube_block_functionality<block_functionality<block::type::stone>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::wood_planks> : public cube_block_functionality<block_functionality<block::type::wood_planks>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 10, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::stone_slab_top> : public slab_block_functionality<block_functionality<block::type::stone_slab_top>> {
        template<block::face FACE>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<FACE, v2u8>(
                []() { return v2u8{8, 0}; },
                []() { return v2u8{8, 0}; },
                []() { return v2u8{9, 0}; },
                []() { return v2u8{9, 0}; },
                []() { return v2u8{8, 0}; },
                []() { return v2u8{8, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::tall_grass> : public foliage_block_functionality<block_functionality<block::type::tall_grass>> {
        BF_MB math::vector2u8 get_uv(bl_st) {
            return { 5, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::water> {
        BF_MB block::category get_category(bl_st) { return block::category::transparent_cube; }

        template<block::face FACE>
        BF_MB bool is_face_invisible_with_neighbor(bl_st st, const block& nb) {
            auto category = get_with_block_functionality<block::category>(nb.tp, [&nb]<typename NB_BF>() {
                return NB_BF::get_category((game::block::state)game::block::slab_state::bottom);
            });
            return
                category == block::category::opaque_cube ||
                category == block::category::transparent_cube ||
                (FACE == block::face::top && category == block::category::opaque_bottom_slab) ||
                (FACE == block::face::bottom && category == block::category::opaque_top_slab);
        }

        template<block::face FACE, typename M, typename F>
        BF_MB void add_face_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos, math::vector3u8 offset_pos) {
            const block* nb_block = get_face_neighbor_block.template operator()<FACE>();
            if (
                nb_block == nullptr ||
                is_face_invisible_with_neighbor<FACE>(st, *nb_block)
            ) {
                return;
            }
            math::vector2u8 uv = math::vector2u8{ 7, 0 } * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
            add_flat_face_vertices<FACE>(ms_st, pos, offset_pos, uv, offset_uv);
        }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            call_func_on_each_face<void>(
                [&]<block::face FACE>() { add_face_vertices<FACE>(ms_st, get_face_neighbor_block, st, pos, offset_pos); }
            );
        }

        template<typename M, typename F>
        BF_MB void add_general_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 1> get_selection_boxes(bl_st) { return {
            math::box{
                { 0.0f, 0.0f, 0.0f },
                { 1.0f, 1.0f, 1.0f }
            }
        }; }

        BF_MB std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::partially_opaque; }
    };
}