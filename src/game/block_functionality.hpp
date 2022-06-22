#pragma once
#include "block.hpp"
#include "util.hpp"
#include "face_mesh_generation_core.hpp"
#include "block_core.hpp"
#include "math/box.hpp"
#include <array>

namespace game {
    template<block::type type>
    struct block_functionality {
        template<block::face face>
        static constexpr bool is_face_visible(block::type) { return false; }

        static constexpr bool is_visible() { return false; }
        static constexpr bool is_fully_transparent() { return true; }
        static constexpr bool is_upper_half_transparent() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { return 0; }

        static constexpr std::size_t get_general_vertex_count() { return 0; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(Vf&, math::vector3u8) { }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf&, math::vector3u8) { }

        static constexpr std::array<math::box, 0> get_boxes() { return {}; }

        static constexpr f32 get_height() { return 0.0f; }
    };

    constexpr u8 block_size = 4;
    constexpr u8 half_block_size = block_size / 2;

    inline math::vector3u8 get_local_pos_offset(math::vector3u8 local_pos, math::vector3u8 offset) {
        return { local_pos.x + offset.x, local_pos.y + offset.y, local_pos.z + offset.z };
    }

    inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos, math::vector2u8 offset) {
        return { uv_pos.x + offset.x, uv_pos.y + offset.y };
    }

    template<typename T>
    struct cube_block_functionality {
        template<block::face face>
        static constexpr bool is_face_visible(block::type nb_tp) {
            if constexpr (face == block::face::TOP) {
                return is_block_upper_half_transparent(nb_tp) && nb_tp != block::type::STONE_SLAB;
            } else {
                return is_block_upper_half_transparent(nb_tp);
            }
        }

        static constexpr bool is_visible() { return true; }
        static constexpr bool is_fully_transparent() { return false; }
        static constexpr bool is_upper_half_transparent() { return false; }
        
        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { return 4; }

        static constexpr std::size_t get_general_vertex_count() { return 0; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(Vf& vf, math::vector3u8 local_pos) {
            auto uv_pos = T::template get_uv_pos<face>();
            uv_pos *= block_size;
            local_pos *= block_size;
            call_face_func_for<face, void>(
                add_cube_front_vertices<Vf>,
                add_cube_back_vertices<Vf>,
                add_cube_top_vertices<Vf>,
                add_cube_bottom_vertices<Vf>,
                add_cube_right_vertices<Vf>,
                add_cube_left_vertices<Vf>,
                vf, local_pos, get_local_pos_offset(local_pos, { block_size, block_size, block_size }), uv_pos, get_uv_position_offset(uv_pos, { block_size, block_size })
            );
        }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf& vf, math::vector3u8 local_pos) { }

        static constexpr std::array<math::box, 1> get_boxes() { return {
            math::box
            {
                .lesser_corner = { 0.0f, 0.0f, 0.0f },
                .greater_corner = { 1.0f, 1.0f, 1.0f }
            }
        }; }

        static constexpr f32 get_height() { return 1.0f; }
    };

    
    template<typename T>
    struct slab_block_functionality {
        template<block::face face>
        static constexpr bool is_face_visible(block::type nb_tp) {
            if constexpr (face == block::face::BOTTOM) {
                return is_block_upper_half_transparent(nb_tp);
            } else {
                return is_block_fully_transparent(nb_tp); 
            }
        }

        static constexpr bool is_visible() { return true; }
        static constexpr bool is_fully_transparent() { return false; }
        static constexpr bool is_upper_half_transparent() { return true; }
        
        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::TOP) return 4; return 0; }

        static constexpr std::size_t get_general_vertex_count() { return 4; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(Vf& vf, math::vector3u8 local_pos) {
            local_pos *= block_size;

            if constexpr (face == block::face::BOTTOM) {
                math::vector2u8 uv_pos = T::template get_uv_pos<face>();
                uv_pos *= block_size;

                add_cube_bottom_vertices(
                    vf,
                    local_pos,
                    get_local_pos_offset(local_pos, { block_size, half_block_size, block_size }),
                    uv_pos,
                    get_uv_position_offset(uv_pos, { block_size, block_size })
                );
            } else if constexpr (face != block::face::TOP) {
                math::vector2u8 uv_pos = T::template get_uv_pos<face>();
                uv_pos *= block_size;
                
                call_face_func_for<face, void>(
                    add_cube_front_vertices<Vf>,
                    add_cube_back_vertices<Vf>,
                    []() {},
                    []() {},
                    add_cube_right_vertices<Vf>,
                    add_cube_left_vertices<Vf>,
                    vf, local_pos, get_local_pos_offset(local_pos, { block_size, half_block_size, block_size }), uv_pos, get_uv_position_offset(uv_pos, { block_size, half_block_size })
                );
            }
        }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf& vf, math::vector3u8 local_pos) {
            local_pos *= block_size;
            auto local_pos_offset = get_local_pos_offset(local_pos, { block_size, half_block_size, block_size });

            math::vector2u8 uv_pos = T::template get_uv_pos<block::face::TOP>();
            uv_pos *= block_size;
            auto uv_pos_offset = get_uv_position_offset(uv_pos, { block_size, block_size });

            add_cube_top_vertices(vf, local_pos, local_pos_offset, uv_pos, uv_pos_offset);
        }

        static constexpr std::array<math::box, 1> get_boxes() { return {
            math::box
            {
                .lesser_corner = { 0.0f, 0.0f, 0.0f },
                .greater_corner = { 1.0f, 0.5f, 1.0f }
            }
        }; }

        static constexpr f32 get_height() { return 0.5f; }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{ 0, 0 }; },
                []() { return v2u8{ 1, 0 }; },
                []() { return v2u8{ 2, 0 }; },
                []() { return v2u8{ 3, 0 }; },
                []() { return v2u8{ 4, 0 }; },
                []() { return v2u8{ 5, 0 }; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::GRASS> : public cube_block_functionality<block_functionality<block::type::GRASS>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{ 3, 0 }; },
                []() { return v2u8{ 3, 0 }; },
                []() { return v2u8{ 0, 0 }; },
                []() { return v2u8{ 2, 0 }; },
                []() { return v2u8{ 3, 0 }; },
                []() { return v2u8{ 3, 0 }; }
            );
        }
    };

    template<>
    struct block_functionality<block::type::DIRT> : public cube_block_functionality<block_functionality<block::type::DIRT>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            return { 2, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE> : public cube_block_functionality<block_functionality<block::type::STONE>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            return { 1, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::WOOD_PLANKS> : public cube_block_functionality<block_functionality<block::type::WOOD_PLANKS>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            return { 4, 0 };
        }
    };

    template<>
    struct block_functionality<block::type::STONE_SLAB> : public slab_block_functionality<block_functionality<block::type::STONE_SLAB>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            using v2u8 = math::vector2u8;
            return call_face_func_for<face, v2u8>(
                []() { return v2u8{ 5, 0 }; },
                []() { return v2u8{ 5, 0 }; },
                []() { return v2u8{ 6, 0 }; },
                []() { return v2u8{ 6, 0 }; },
                []() { return v2u8{ 5, 0 }; },
                []() { return v2u8{ 5, 0 }; }
            );
        }
    };
}