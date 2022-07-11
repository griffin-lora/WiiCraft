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

template<typename Bf, block::face face>
static bool should_add_vertices_for_face(const block* blocks, const chunk::neighborhood& nh, bl_st block_state, const math::vector3s32& local_pos) {
    if (is_block_position_at_face_edge<face>(local_pos)) {
        // We are at the edge of the block, so we should check the neighbor chunk.
        auto nb = get_neighbor<face>(nh);
        if (nb.has_value()) {
            math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
            face_block_pos = get_local_block_position_in_s32(face_block_pos);
            
            auto& block = nb->get().blocks[get_index_from_position(face_block_pos)];
            return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
        }

        return false;
    }
    math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
    
    auto& block = blocks[get_index_from_position(face_block_pos)];
    return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
}

template<typename Bf, block::face face, typename Vf>
static void add_face_vertices_if_needed(const block* blocks, const chunk::neighborhood& nh, bl_st block_state, const math::vector3s32& pos, Vf& vf, math::vector3u8 block_pos) {
    if (Bf::template get_face_traits<face>(block_state).visible && should_add_vertices_for_face<Bf, face>(blocks, nh, block_state, pos)) {
        Bf::template add_face_vertices<face>(vf, block_pos, block_state);
    }
}

void game::update_mesh(const ext::data_array<chunk::block_position_index_pair>& position_index_pairs, standard_quad_building_arrays& building_arrays, chunk& chunk) {
    auto blocks = chunk.blocks.data();
    auto& nh = chunk.nh;

    standard_vertex_function vf = {
        .it = { building_arrays }
    };

    standard_quad_iterators begin = { building_arrays };

    for (auto& [ block_pos, index ] : position_index_pairs) {
        auto& block = blocks[index];

        call_with_block_functionality(block.tp, [&]<typename Bf>() {
            if (Bf::get_block_traits(block.st).visible) {
                math::vector3u8 pos = block_pos;
                #define ADD_FACE_VERTICES_IF_NEEDED_PARAMS blocks, nh, block.st, block_pos, vf, pos
                add_face_vertices_if_needed<Bf, block::face::FRONT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::BACK>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::TOP>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::BOTTOM>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::RIGHT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                add_face_vertices_if_needed<Bf, block::face::LEFT>(ADD_FACE_VERTICES_IF_NEEDED_PARAMS);
                Bf::add_general_vertices(vf, block_pos, block.st);
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