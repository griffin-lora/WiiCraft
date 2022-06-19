#pragma once
#include "block.hpp"
#include "block_core.hpp"
#include "face_mesh_generation_core.hpp"

namespace game {
    template<block::type type>
    struct block_functionality {
        static constexpr bool is_solid() { return false; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { return 0; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(math::vector3u8 local_position) { }
    };

    inline math::vector3u8 get_local_pos_offset(math::vector3u8 local_pos) {
        return { local_pos.x + 1, local_pos.y + 1, local_pos.z + 1 };
    }

    inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos) {
        return { uv_pos.x + 1, uv_pos.y + 1 };
    }

    template<typename T>
    struct cube_block_functionality {
        static constexpr bool is_solid() { return true; }
        
        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::CENTER) return 4; return 0; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(math::vector3u8 local_pos) {
            static_assert(face != block::face::CENTER, "Center face is not allowed.");
            auto uv_pos = T::template get_uv_pos<face>();
            call_face_func_for<face, void>(
                add_cube_front_vertices<Vf>,
                add_cube_back_vertices<Vf>,
                add_cube_top_vertices<Vf>,
                add_cube_bottom_vertices<Vf>,
                add_cube_right_vertices<Vf>,
                add_cube_left_vertices<Vf>,
                []() {},
                local_pos, get_local_pos_offset(local_pos), uv_pos, get_uv_position_offset(uv_pos)
            );
        }
    };

    template<>
    struct block_functionality<block::type::DEBUG> : public cube_block_functionality<block_functionality<block::type::DEBUG>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            static_assert(face != block::face::CENTER, "Center face is not allowed.");
            return call_face_func_for<face, math::vector2u8>(
                []() -> math::vector2u8 { return { 0, 0 }; },
                []() -> math::vector2u8 { return { 1, 0 }; },
                []() -> math::vector2u8 { return { 2, 0 }; },
                []() -> math::vector2u8 { return { 3, 0 }; },
                []() -> math::vector2u8 { return { 4, 0 }; },
                []() -> math::vector2u8 { return { 5, 0 }; },
                []() {}
            );
        }
    };

    template<>
    struct block_functionality<block::type::GRASS> : public cube_block_functionality<block_functionality<block::type::GRASS>> {
        template<block::face face>
        static constexpr math::vector2u8 get_uv_pos() {
            static_assert(face != block::face::CENTER, "Center face is not allowed.");
            return call_face_func_for<face, math::vector2u8>(
                []() -> math::vector2u8 { return { 3, 0 }; },
                []() -> math::vector2u8 { return { 3, 0 }; },
                []() -> math::vector2u8 { return { 0, 0 }; },
                []() -> math::vector2u8 { return { 2, 0 }; },
                []() -> math::vector2u8 { return { 3, 0 }; },
                []() -> math::vector2u8 { return { 3, 0 }; },
                []() {}
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
}