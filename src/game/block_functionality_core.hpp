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
        template<typename M, typename F>
        BF_MB void add_vertices(M&, const F&, bl_st, math::vector3u8) {}

        BF_MB std::array<math::box, 0> get_selection_boxes(bl_st) { return {}; }
        BF_MB std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        BF_MB block_counting_type get_block_counting_type(bl_st) { return block_counting_type::INVISIBLE; }
    };

    template<typename Bf>
    struct cube_block_functionality {
        template<typename M, typename F>
        BF_MB void add_vertices(M&, const F&, bl_st, math::vector3u8) {}

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
        template<typename M, typename F>
        BF_MB void add_vertices(M&, const F&, bl_st, math::vector3u8) {}

        using state = block::slab_state;

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
        template<typename M, typename F>
        BF_MB void add_vertices(M&, const F&, bl_st, math::vector3u8) {}

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
        BF_MB math::vector2u8 get_uv_position(bl_st) {
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
        BF_MB math::vector2u8 get_uv_position(bl_st) {
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
        BF_MB math::vector2u8 get_uv_position(bl_st) {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::SAND> : public cube_block_functionality<block_functionality<block::type::SAND>> {
        template<block::face face>
        BF_MB math::vector2u8 get_uv_position(bl_st) {
            return { 2, 1 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE> : public cube_block_functionality<block_functionality<block::type::STONE>> {
        template<block::face face>
        BF_MB math::vector2u8 get_uv_position(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::WOOD_PLANKS> : public cube_block_functionality<block_functionality<block::type::WOOD_PLANKS>> {
        template<block::face face>
        BF_MB math::vector2u8 get_uv_position(bl_st) {
            return { 4, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE_SLAB> : public slab_block_functionality<block_functionality<block::type::STONE_SLAB>> {
        template<block::face face>
        BF_MB math::vector2u8 get_uv_position(bl_st) {
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
        BF_MB math::vector2u8 get_uv_position(bl_st) {
            return { 7, 2 };
        }
    };

    template<>
    struct block_functionality<block::type::WATER> {
        template<typename M, typename F>
        BF_MB void add_vertices(M&, const F&, bl_st, math::vector3u8) {}

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