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
static bool should_add_vertices_for_face(const chunk& chunk, const block::state& block_state, const math::vector3s32& local_pos) {
    if (is_block_position_at_face_edge<face>(local_pos)) {
        // We are at the edge of the block, so we should check the neighbor chunk.
        auto nb = get_neighbor<face>(chunk.nh);
        if (nb.has_value()) {
            math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
            face_block_pos = get_local_block_position_in_s32(face_block_pos);
            
            auto& block = nb->get().blocks[get_index_from_position(face_block_pos)];
            return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
        }

        return false;
    }
    math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
    
    auto& block = chunk.blocks[get_index_from_position(face_block_pos)];
    return Bf::template is_face_visible_with_neighbor<face>(block_state, block);
}

template<typename Bf, block::face face, typename Vf>
static void add_face_vertices_if_needed(const chunk& chunk, Vf& vf, math::vector3u8 block_pos, const block::state& block_state, const math::vector3s32& pos) {
    if (Bf::template get_face_traits<face>(block_state).visible && should_add_vertices_for_face<Bf, face>(chunk, block_state, pos)) {
        Bf::template add_face_vertices<face>(vf, block_pos, block_state);
    }
}

void game::update_mesh(chunk& chunk, ext::data_array<chunk::quad>& building_quads) {
    standard_vertex_function vf = {
        .quad_it = building_quads.begin()
    };

    for (u8 x = 0; x < (u8)chunk::SIZE; x++) {
        for (u8 y = 0; y < (u8)chunk::SIZE; y++) {
            for (u8 z = 0; z < (u8)chunk::SIZE; z++) {
                math::vector3u8 block_pos = {x, y, z};
                auto& block = chunk.blocks[get_index_from_position(block_pos)];

                call_with_block_functionality(block.tp, [&]<typename Bf>() {
                    if (Bf::get_block_traits(block.st).visible) {
                        math::vector3u8 pos = block_pos;
                        add_face_vertices_if_needed<Bf, block::face::FRONT>(chunk, vf, block_pos, block.st, pos);
                        add_face_vertices_if_needed<Bf, block::face::BACK>(chunk, vf, block_pos, block.st, pos);
                        add_face_vertices_if_needed<Bf, block::face::TOP>(chunk, vf, block_pos, block.st, pos);
                        add_face_vertices_if_needed<Bf, block::face::BOTTOM>(chunk, vf, block_pos, block.st, pos);
                        add_face_vertices_if_needed<Bf, block::face::RIGHT>(chunk, vf, block_pos, block.st, pos);
                        add_face_vertices_if_needed<Bf, block::face::LEFT>(chunk, vf, block_pos, block.st, pos);
                        Bf::add_general_vertices(vf, block_pos, block.st);
                    }
                });

                std::size_t total_quad_count = vf.quad_it - building_quads.begin();
                if (total_quad_count > chunk::MAX_QUAD_COUNT) {
                    dbg::error([total_quad_count]() {
                        printf("Chunk quad count is too high: %d\n", total_quad_count);
                    });
                }
            }
        }
    }

    write_into_display_lists(building_quads, vf, chunk.standard_disp_list, chunk.foliage_disp_list, [](auto vert_count) {
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