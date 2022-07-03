#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"
#include "math/box.hpp"
#include "traits.hpp"
#include "util.hpp"
#include <array>

namespace game {
    #define BF_FUNC static constexpr

    template<block::type type>
    struct block_functionality {
        BF_FUNC block_traits get_block_traits(bl_st) { return {}; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {}; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block&) { return false; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf&, math::vector3u8, bl_st) {}

        BF_FUNC std::array<math::box, 0> get_selection_boxes(bl_st) { return {}; }
        BF_FUNC std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }
    };

    template<typename Bf>
    struct cube_block_functionality {
        using self = cube_block_functionality<Bf>;

        BF_FUNC block_traits get_block_traits(bl_st) { return {
            .visible = true
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {
            .visible = true,
            .partially_transparent = false,
            .bottom_half_transparent = false,
            .top_half_transparent = false,
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block& block) { return get_neighbor_face_traits<face>(block).partially_transparent; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            add_flat_face_vertices_from_block_position<face, self>(vf, block_pos, st);
        }

        BF_FUNC std::array<math::box, 1> get_boxes() {
            return {
                math::box{
                    .lesser_corner = { 0.0f, 0.0f, 0.0f },
                    .greater_corner = { 1.0f, 1.0f, 1.0f }
                }
            };
        }

        BF_FUNC auto get_selection_boxes(bl_st) { return get_boxes(); }
        BF_FUNC auto get_collision_boxes(bl_st) { return get_boxes(); }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st st) {
            return Bf::template get_uv_position<face>(st);
        }

        BF_FUNC draw_positions get_draw_positions(const draw_positions& draw_positions, bl_st) { return draw_positions; }
        
        template<block::face face>
        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& draw_positions, bl_st) {
            return {
                .block_draw_pos = draw_positions.block_draw_pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size },
                .uv_draw_pos = draw_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size }
            };
        }
    };

    template<typename Bf>
    struct slab_block_functionality {
        using self = slab_block_functionality<Bf>;
        using state = block::slab_state;

        BF_FUNC block_traits get_block_traits(bl_st st) { return {
            .visible = true
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            switch (st.slab) {
                default: break;
                case state::BOTTOM: add_flat_face_vertices_from_block_position<block::face::TOP, self>(vf, block_pos, st); break;
                case state::TOP: add_flat_face_vertices_from_block_position<block::face::BOTTOM, self>(vf, block_pos, st); break;
            }
        }

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st st) { return {
            .visible = [&st]() {
                switch (st.slab) {
                    default: return true;
                    case state::BOTTOM: if constexpr (face == block::face::TOP) return false; return true;
                    case state::TOP: if constexpr (face == block::face::BOTTOM) return false; return true;
                }
            }(),
            .partially_transparent = [&st]() {
                switch (st.slab) {
                    default: return false;
                    case state::BOTTOM: if constexpr (face == block::face::BOTTOM) return false; return true;
                    case state::TOP: if constexpr (face == block::face::TOP) return false; return true;
                }
            }(),
            .bottom_half_transparent = (st.slab == state::TOP),
            .top_half_transparent = (st.slab == state::BOTTOM)
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st st, const block& block) {
            auto traits = get_neighbor_face_traits<face>(block);
            switch (st.slab) {
                case state::BOTTOM: if constexpr (face != block::face::BOTTOM) return traits.bottom_half_transparent; break;
                case state::TOP: if constexpr (face != block::face::TOP) return traits.top_half_transparent; break;
                default: break;
            }
            return traits.partially_transparent;
        }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            add_flat_face_vertices_from_block_position<face, self>(vf, block_pos, st);
        }

        BF_FUNC std::array<math::box, 1> get_boxes(bl_st st) {
            return {
                math::box{
                    .lesser_corner = { 0.0f, (st.slab == state::TOP ? 0.5f : 0.0f), 0.0f },
                    .greater_corner = { 1.0f, (st.slab == state::BOTTOM ? 0.5f : 1.0f), 1.0f }
                }
            };
        }

        BF_FUNC auto get_selection_boxes(bl_st st) { return get_boxes(st); }
        BF_FUNC auto get_collision_boxes(bl_st st) { return get_boxes(st); }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st st) {
            return Bf::template get_uv_position<face>(st);
        }

        BF_FUNC draw_positions get_draw_positions(const draw_positions& d_positions, bl_st st) {
            return (st.slab == state::TOP ? draw_positions{
                .block_draw_pos = d_positions.block_draw_pos + math::vector3u8{ 0, half_block_draw_size, 0 },
                .uv_draw_pos = d_positions.uv_draw_pos
            } : d_positions);
        }

        template<block::face face>
        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& d_positions, bl_st st) {
            return {
                .block_draw_pos = d_positions.block_draw_pos + math::vector3u8{ block_draw_size, (st.slab == state::BOTH ? block_draw_size : half_block_draw_size), block_draw_size },
                .uv_draw_pos = [&d_positions, &st]() -> math::vector2u8 {
                    if constexpr (face == block::face::TOP || face == block::face::BOTTOM) {
                        return d_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size };
                    }
                    return d_positions.uv_draw_pos + math::vector2u8{ block_draw_size, (st.slab == state::BOTH ? block_draw_size : half_block_draw_size) };
                }()
            };
        }
    };
    
    template<typename Bf>
    struct foliage_block_functionality {
        BF_FUNC block_traits get_block_traits(bl_st) { return {
            .visible = true
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            draw_positions d_positions = {
                .block_draw_pos = block_pos * block_draw_size,
                .uv_draw_pos = Bf::get_uv_position(st) * block_draw_size
            };

            draw_positions offset_d_positions = {
                .block_draw_pos = d_positions.block_draw_pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size },
                .uv_draw_pos = d_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size }
            };
            add_foliage_vertices(vf, d_positions, offset_d_positions);
        }

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {
            .visible = true
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block&) { return false; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf&, math::vector3u8, bl_st) {}

        BF_FUNC std::array<math::box, 1> get_selection_boxes(bl_st) {
            return {
                math::box{
                    .lesser_corner = { 0.2f, 0.0f, 0.2f },
                    .greater_corner = { 0.8f, 0.8f, 0.8f }
                }
            };
        }
        BF_FUNC std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
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
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
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
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE> : public cube_block_functionality<block_functionality<block::type::STONE>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::WOOD_PLANKS> : public cube_block_functionality<block_functionality<block::type::WOOD_PLANKS>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 4, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE_SLAB> : public slab_block_functionality<block_functionality<block::type::STONE_SLAB>> {
        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::TALL_GRASS> : public foliage_block_functionality<block_functionality<block::type::TALL_GRASS>> {
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 7, 2 };
        }
    };

    template<>
    struct block_functionality<block::type::WATER> {
        using self = block_functionality<block::type::WATER>;

        BF_FUNC block_traits get_block_traits(bl_st) { return {
            .visible = true
        }; }

        template<typename Vf>
        BF_FUNC void add_general_vertices(Vf&, math::vector3u8, bl_st) {}

        template<block::face face>
        BF_FUNC face_traits get_face_traits(bl_st) { return {
            .visible = true
        }; }

        template<block::face face>
        BF_FUNC bool is_face_visible_with_neighbor(bl_st, const block& bl) { return bl.tp != block::type::WATER && get_neighbor_face_traits<face>(bl).partially_transparent; }

        template<block::face face, typename Vf>
        BF_FUNC void add_face_vertices(Vf& vf, math::vector3u8 block_pos, bl_st st) {
            add_flat_face_vertices_from_block_position<face, self>(vf, block_pos, st);
        }

        BF_FUNC std::array<math::box, 1> get_selection_boxes(bl_st) { return {
            math::box{
                .lesser_corner = { 0.0f, 0.0f, 0.0f },
                .greater_corner = { 1.0f, 1.0f, 1.0f }
            }
        }; }

        BF_FUNC std::array<math::box, 0> get_collision_boxes(bl_st) { return {}; }

        template<block::face face>
        BF_FUNC math::vector2u8 get_uv_position(bl_st) {
            return { 13, 12 };
        }

        BF_FUNC draw_positions get_draw_positions(const draw_positions& draw_positions, bl_st) { return draw_positions; }
        
        template<block::face face>
        BF_FUNC draw_positions get_offset_draw_positions(const draw_positions& draw_positions, bl_st) {
            return {
                .block_draw_pos = draw_positions.block_draw_pos + math::vector3u8{ block_draw_size, block_draw_size, block_draw_size },
                .uv_draw_pos = draw_positions.uv_draw_pos + math::vector2u8{ block_draw_size, block_draw_size }
            };
        }
    };
}