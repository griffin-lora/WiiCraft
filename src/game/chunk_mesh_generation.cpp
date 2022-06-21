#include "chunk_mesh_generation.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "chunk_math.hpp"
#include "block_core.hpp"
#include "block_core.inl"
#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static bool should_add_vertices_for_face(const chunk& chunk, math::vector3s32 local_pos, block::type type) {
    if (is_block_position_at_face_edge<face>(local_pos)) {
        // We are at the edge of the block, so we should check the neighbor chunk.
        auto nb = get_neighbor<face>(chunk.nh);
        if (nb.has_value()) {
            math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
            face_block_pos = get_local_block_position_in_s32(face_block_pos);
            
            auto& block = nb->get().blocks[get_index_from_position(face_block_pos)];
            return is_block_face_visible<face>(type, block.tp);
        }

        return false;
    } else {
        math::vector3s32 face_block_pos = get_face_offset_position<face>(local_pos);
        
        auto& block = chunk.blocks[get_index_from_position(face_block_pos)];
        return is_block_face_visible<face>(type, block.tp);
    }
}

template<block::face face, typename Vf>
static void add_face_vertices_if_needed(const chunk& chunk, Vf& vf, math::vector3s32 local_pos, block::type type) {
    if (should_add_vertices_for_face<face>(chunk, local_pos, type)) {
        add_face_vertices<face>(vf, local_pos, type);
    }
}

void game::update_mesh(chunk& chunk, ext::data_array<chunk::vertex>& building_verts) {
    auto vert_it = building_verts.begin();
    auto vf = [&vert_it](u8 x, u8 y, u8 z, u8 u, u8 v) {
        *vert_it++ = {
            .pos = { x, y, z },
            .uv = { u, v }
        };
    };

    iterate_over_chunk_positions_and_blocks<s32>(chunk.blocks, [&chunk, &building_verts, &vert_it, &vf](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_face_vertices_if_needed<block::face::FRONT>(chunk, vf, pos, type);
            add_face_vertices_if_needed<block::face::BACK>(chunk, vf, pos, type);
            add_face_vertices_if_needed<block::face::TOP>(chunk, vf, pos, type);
            add_face_vertices_if_needed<block::face::BOTTOM>(chunk, vf, pos, type);
            add_face_vertices_if_needed<block::face::RIGHT>(chunk, vf, pos, type);
            add_face_vertices_if_needed<block::face::LEFT>(chunk, vf, pos, type);
            add_general_vertices(vf, pos, type);
        }
        std::size_t vertex_count = vert_it - building_verts.begin();
        if (vertex_count > chunk::MAX_VERTEX_COUNT) {
            dbg::error([vertex_count]() {
                printf("Chunk vertex count is too high: %d\n", vertex_count);
            });
        }
    });

    std::size_t vertex_count = vert_it - building_verts.begin();

    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 3 + // GX_Position3u8
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    chunk.disp_list.resize(disp_list_size);

    chunk.disp_list.write_into([&building_verts, &vert_it, vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        for (auto it = building_verts.begin(); it != vert_it; ++it) {
            GX_Position3u8(it->pos.x, it->pos.y, it->pos.z);
            GX_TexCoord2u8(it->uv.x, it->uv.y);
        }
        
        GX_End();
    });
}