#pragma once
#include "block.hpp"
#include "util.hpp"
#include "face_mesh_generation_core.hpp"
#include "block_core.hpp"

namespace game {
    template<block::type type>
    struct block_functionality {
        template<block::face face>
        static constexpr bool is_face_visible(block::type) { return false; }

        static constexpr bool is_visible() { return false; }
        static constexpr bool is_semitransparent() { return true; }

        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { return 0; }

        static constexpr std::size_t get_general_vertex_count() { return 0; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(Vf&, math::vector3u8) { }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf&, math::vector3u8) { }
    };

    constexpr u8 block_size = 4;
    constexpr u8 half_block_size = block_size / 2;

    inline math::vector3u8 get_local_pos_offset(math::vector3u8 local_pos, u8 offset) {
        return { local_pos.x + offset, local_pos.y + offset, local_pos.z + offset };
    }

    inline math::vector2u8 get_uv_position_offset(math::vector2u8 uv_pos, u8 offset) {
        return { uv_pos.x + offset, uv_pos.y + offset };
    }

    template<typename T>
    struct cube_block_functionality {
        template<block::face face>
        static constexpr bool is_face_visible(block::type tp) { return is_block_semitransparent(tp); }

        static constexpr bool is_visible() { return true; }
        static constexpr bool is_semitransparent() { return false; }
        
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
                vf, local_pos, get_local_pos_offset(local_pos, block_size), uv_pos, get_uv_position_offset(uv_pos, block_size)
            );
        }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf& vf, math::vector3u8 local_pos) { }
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
    struct block_functionality<block::type::STONE_SLAB> {
        template<block::face face>
        static constexpr bool is_face_visible(block::type tp) { return is_block_semitransparent(tp) && tp != block::type::STONE_SLAB; }

        static constexpr bool is_visible() { return true; }
        static constexpr bool is_semitransparent() { return true; }
        
        template<block::face face>
        static constexpr std::size_t get_face_vertex_count() { if constexpr (face != block::face::TOP) return 4; return 0; }

        static constexpr std::size_t get_general_vertex_count() { return 4; }

        template<block::face face, typename Vf>
        static constexpr void add_face_vertices(Vf& vf, math::vector3u8 local_pos) {
            local_pos *= block_size;

            auto local_pos_offset = local_pos;
            local_pos_offset.x += block_size;
            local_pos_offset.y += half_block_size;
            local_pos_offset.z += block_size;
            if constexpr (face == block::face::BOTTOM) {
                math::vector2u8 uv_pos = { 6, 0 };
                uv_pos *= block_size;
                auto uv_pos_offset = get_uv_position_offset(uv_pos, block_size);

                call_face_func_for<face, void>(
                    []() {},
                    []() {},
                    []() {},
                    add_cube_bottom_vertices<Vf>,
                    []() {},
                    []() {},
                    vf, local_pos, local_pos_offset, uv_pos, uv_pos_offset
                );
            } else if constexpr (face != block::face::TOP) {
                math::vector2u8 uv_pos = { 5, 0 };
                uv_pos *= block_size;
                
                auto uv_pos_offset = uv_pos;
                uv_pos_offset.x += block_size;
                uv_pos_offset.y += half_block_size;
                
                call_face_func_for<face, void>(
                    add_cube_front_vertices<Vf>,
                    add_cube_back_vertices<Vf>,
                    []() {},
                    []() {},
                    add_cube_right_vertices<Vf>,
                    add_cube_left_vertices<Vf>,
                    vf, local_pos, local_pos_offset, uv_pos, uv_pos_offset
                );
            }
        }

        template<typename Vf>
        static constexpr void add_general_vertices(Vf& vf, math::vector3u8 local_pos) {
            local_pos *= block_size;

            auto local_pos_offset = local_pos;
            local_pos_offset.x += block_size;
            local_pos_offset.y += half_block_size;
            local_pos_offset.z += block_size;

            math::vector2u8 uv_pos = { 6, 0 };
            uv_pos *= block_size;
            auto uv_pos_offset = get_uv_position_offset(uv_pos, block_size);

            add_cube_top_vertices(vf, local_pos, local_pos_offset, uv_pos, uv_pos_offset);
        }
    };
}