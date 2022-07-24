#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "block_util.hpp"
#include <array>

namespace game {
    #define BF_MB static constexpr

    template<block::type type>
    struct block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::TRANSPARENT; }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M&, const F&, bl_st, math::vector3u8) {}

        template<typename M, typename F>
        BF_MB void add_general_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 0> get_selection_boxes(bl_st) { return {}; }
        BF_MB std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::INVISIBLE; }
    };

    template<typename Bf>
    struct cube_block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::OPAQUE_CUBE; }

        template<block::face face>
        BF_MB bool is_face_invisible_with_neighbor(bl_st st, const block& nb) {
            auto category = get_with_block_functionality<block::category>(nb.tp, [&nb]<typename N_Bf>() {
                return N_Bf::get_category(nb.st);
            });
            return
                category == block::category::OPAQUE_CUBE ||
                (face == block::face::TOP && category == block::category::OPAQUE_BOTTOM_SLAB) ||
                (face == block::face::BOTTOM && category == block::category::OPAQUE_TOP_SLAB);
        }

        template<block::face face, typename M, typename F>
        BF_MB void add_face_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos, math::vector3u8 offset_pos) {
            const block* nb_block = get_face_neighbor_block.template operator()<face>();
            if (
                nb_block == nullptr ||
                is_face_invisible_with_neighbor<face>(st, *nb_block)
            ) {
                return;
            }
            math::vector2u8 uv = Bf::template get_face_uv<face>(st/* TODO: add this param: , *nb_block */) * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
            add_flat_face_vertices<face, M, &M::add_standard>(ms_st, pos, offset_pos, uv, offset_uv);
        }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            call_func_on_each_face<void>(
                [&]<block::face face>() { add_face_vertices<face>(ms_st, get_face_neighbor_block, st, pos, offset_pos); }
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

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::FULLY_OPAQUE; }
    };

    template<typename Bf>
    struct slab_block_functionality {
        using state = block::slab_state;

        BF_MB block::category get_category(bl_st st) {
            switch (st.slab) {
                default: return block::category::OPAQUE_CUBE;
                case state::TOP: return block::category::OPAQUE_TOP_SLAB;
                case state::BOTTOM: return block::category::OPAQUE_BOTTOM_SLAB;
            }
        }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M&, const F&, bl_st, math::vector3u8) {}

        template<typename M, typename F>
        BF_MB void add_general_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 1> get_boxes(bl_st st) {
            return {
                math::box{
                    { 0.0f, (st.slab == state::TOP ? 0.5f : 0.0f), 0.0f },
                    { 1.0f, (st.slab == state::BOTTOM ? 0.5f : 1.0f), 1.0f }
                }
            };
        }

        BF_MB auto get_selection_boxes(bl_st st) { return get_boxes(st); }
        BF_MB auto get_collision_boxes(bl_st st) { return get_boxes(st); }

        BF_MB block_counting_type get_block_counting_type(bl_st st) { return (
            st.slab == state::BOTH ?
            block_counting_type::FULLY_OPAQUE :
            block_counting_type::PARTIALLY_OPAQUE
        ); }
    };
    
    template<typename Bf>
    struct foliage_block_functionality {
        BF_MB block::category get_category(bl_st) { return block::category::TRANSPARENT; }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M&, const F&, bl_st, math::vector3u8) {}

        template<typename M, typename F>
        BF_MB void add_general_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            math::vector2u8 uv = Bf::get_uv(st) * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };

            add_foliage_vertices<M, &M::add_foliage>(ms_st, pos, offset_pos, uv, offset_uv);
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

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::PARTIALLY_OPAQUE; }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
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
    struct block_functionality<block::type::GRASS> : public cube_block_functionality<block_functionality<block::type::GRASS>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{3, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{0, 0}; },
                []() { return v2u8{2, 0}; },
                []() { return v2u8{3, 0}; },
                []() { return v2u8{3, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::DIRT> : public cube_block_functionality<block_functionality<block::type::DIRT>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::SAND> : public cube_block_functionality<block_functionality<block::type::SAND>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 2, 1 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE> : public cube_block_functionality<block_functionality<block::type::STONE>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::WOOD_PLANKS> : public cube_block_functionality<block_functionality<block::type::WOOD_PLANKS>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            return { 4, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE_SLAB> : public slab_block_functionality<block_functionality<block::type::STONE_SLAB>> {
        template<block::face face>
        BF_MB math::vector2u8 get_face_uv(bl_st) {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{5, 0}; },
                []() { return v2u8{5, 0}; },
                []() { return v2u8{6, 0}; },
                []() { return v2u8{6, 0}; },
                []() { return v2u8{5, 0}; },
                []() { return v2u8{5, 0}; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::TALL_GRASS> : public foliage_block_functionality<block_functionality<block::type::TALL_GRASS>> {
        BF_MB math::vector2u8 get_uv(bl_st) {
            return { 7, 2 };
        }
    };

    template<>
    struct block_functionality<block::type::WATER> {
        BF_MB block::category get_category(bl_st) { return block::category::TRANSPARENT_CUBE; }

        template<block::face face>
        BF_MB bool is_face_invisible_with_neighbor(bl_st st, const block& nb) {
            auto category = get_with_block_functionality<block::category>(nb.tp, [&nb]<typename N_Bf>() {
                return N_Bf::get_category(nb.st);
            });
            return
                category == block::category::OPAQUE_CUBE ||
                category == block::category::TRANSPARENT_CUBE ||
                (face == block::face::TOP && category == block::category::OPAQUE_BOTTOM_SLAB) ||
                (face == block::face::BOTTOM && category == block::category::OPAQUE_TOP_SLAB);
        }

        template<block::face face, typename M, typename F>
        BF_MB void add_face_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos, math::vector3u8 offset_pos) {
            const block* nb_block = get_face_neighbor_block.template operator()<face>();
            if (
                nb_block == nullptr ||
                is_face_invisible_with_neighbor<face>(st, *nb_block)
            ) {
                return;
            }
            math::vector2u8 uv = math::vector2u8{ 13, 12 } * block_draw_size;
            math::vector2u8 offset_uv = uv + math::vector2u8{ block_draw_size, block_draw_size };
            add_flat_face_vertices<face, M, &M::add_water>(ms_st, pos, offset_pos, uv, offset_uv);
        }

        template<typename M, typename F>
        BF_MB void add_faces_vertices(M& ms_st, const F& get_face_neighbor_block, bl_st st, math::vector3u8 pos) {
            pos *= block_draw_size;
            auto offset_pos = pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size };

            call_func_on_each_face<void>(
                [&]<block::face face>() { add_face_vertices<face>(ms_st, get_face_neighbor_block, st, pos, offset_pos); }
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

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::PARTIALLY_OPAQUE; }
    };
}