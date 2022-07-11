#include "chunk_mesh_generation.hpp"
#include "block_core.hpp"
#include "block_core.inl"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "mesh_generation.hpp"
#include "mesh_generation.inl"
#include <cstdio>

using namespace game;

template<block::face face>
static constexpr std::pair<u16, bool> get_block_lookup_neighbor(const block::lookup::neighborhood& block_nh) {
    return call_face_func_for<face, std::pair<u16, bool>>(
        [&]() { return std::make_pair(block_nh.front, block_nh.is_front_edge); },
        [&]() { return std::make_pair(block_nh.back, block_nh.is_back_edge); },
        [&]() { return std::make_pair(block_nh.top, block_nh.is_top_edge); },
        [&]() { return std::make_pair(block_nh.bottom, block_nh.is_bottom_edge); },
        [&]() { return std::make_pair(block_nh.right, block_nh.is_right_edge); },
        [&]() { return std::make_pair(block_nh.left, block_nh.is_left_edge); }
    );
}

template<typename Bf, block::face face>
static bool should_add_vertices_for_face(const block* blocks, const chunk::neighborhood& chunk_nh, const block::lookup::neighborhood& block_nh, bl_st block_state, const math::vector3s32& local_pos) {
    auto [ index, is_face_edge ] = get_block_lookup_neighbor<face>(block_nh);
    if (is_face_edge) {
        // We are at the edge of the block, so we should check the neighbor chunk.
        auto nb_chunk = get_neighbor<face>(chunk_nh);
        if (nb_chunk.has_value()) {
            auto& block = nb_chunk->get().blocks[index];
            return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
        }

        return false;
    }
    auto& block = blocks[index];
    return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
}

template<typename Bf, block::face face, typename Vf>
static void add_face_vertices_if_needed(const block* blocks, const chunk::neighborhood& chunk_nh, const block::lookup::neighborhood& block_nh, bl_st block_state, const math::vector3s32& pos, Vf& vf, math::vector3u8 block_pos) {
    if (Bf::template get_face_traits<face>(block_state).visible && should_add_vertices_for_face<Bf, face>(blocks, chunk_nh, block_nh, block_state, pos)) {
        Bf::template add_face_vertices<face>(vf, block_pos, block_state);
    }
}

void game::update_mesh(const block::lookups& lookups, standard_quad_building_arrays& building_arrays, chunk& chunk) {
    auto blocks = chunk.blocks.data();
    auto& chunk_nh = chunk.nh;

    standard_vertex_function vf = {
        .it = { building_arrays }
    };

    standard_quad_iterators begin = { building_arrays };

    for (auto it = lookups.begin(); it != lookups.end(); it++) {
        u16 index = it - lookups.begin();
        auto& block = blocks[index];

        call_with_block_functionality(block.tp, [&]<typename Bf>() {
            if (Bf::get_block_traits(block.st).visible) {
                auto& block_nh = it->nh;
                math::vector3s32 pos = it->position;
                #define ADD_FACE_VERTICES_IF_NEEDED_PARAMS blocks, chunk_nh, block_nh, block.st, pos, vf, it->position
                add_face_vertices_if_needed<Bf, block::face::FRONT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::BACK>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::TOP>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::BOTTOM>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::RIGHT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::LEFT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                Bf::add_general_vertices(vf, it->position, block.st);
            }
        });

        if (
            (vf.it.standard - begin.standard) > chunk::MAX_STANDARD_QUAD_COUNT ||
            (vf.it.foliage - begin.foliage) > chunk::MAX_FOLIAGE_QUAD_COUNT ||
            (vf.it.water - begin.water) > chunk::MAX_WATER_QUAD_COUNT
        ) {
            dbg::error([]() {
                printf("Chunk quad count is too high\n");
            });
        }
    }

    write_into_display_lists(begin, vf, chunk.standard_disp_list, chunk.foliage_disp_list, chunk.water_disp_list, [](auto vert_count) {
        return (
            4 + // GX_Begin
            vert_count * 3 + // GX_Position3u8
            vert_count * 2 + // GX_TexCoord2u8
		    1 // GX_End
        );
    }, [](auto& vert) {
        GX_Position3u8(vert.pos.x, vert.pos.y, vert.pos.z);
        GX_TexCoord2u8(vert.uv.x, vert.uv.y);
    });
}