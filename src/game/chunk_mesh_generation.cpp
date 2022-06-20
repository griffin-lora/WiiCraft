#include "chunk_mesh_generation.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "chunk_math.hpp"
#include "is_block_face_visible.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static bool should_render_face(const chunk& chunk, math::vector3u8 pos, block::type type) {
    if (get_face_vertex_count<face>(type) == 0) {
        return false;
    }
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
        // Since the position is outside of the chunk, we need to check the neighbor chunk
        auto nb_chunk_opt = get_neighbor<face>(chunk.nh);

        if (nb_chunk_opt.has_value()) {
            auto& nb_chunk = nb_chunk_opt->get();

            auto nb_check_pos = get_local_block_position(get_face_offset_position<face, math::vector3s32>(pos));

            auto index = get_index_from_position(nb_check_pos);

            auto& block = nb_chunk.blocks[index];
            if (is_block_face_visible<face>(type, block.tp)) {
                return false;
            }
        } else {
            // Don't render chunk faces if there is no neighbor chunk
            return false;
        }
        return true;
    }
    auto check_block_type = chunk.blocks[get_index_from_position(check_pos)].tp;
    if (is_block_face_visible<face>(type, check_block_type)) {
        return false;
    } else {
        return true;
    }
}

template<block::face face, typename Vf>
static bool add_needed_face_vertices(const chunk& chunk, Vf& vf, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, pos, type)) {
        add_face_vertices<face>(vf, pos, type);
        return true;
    }
    return false;
}

void game::update_mesh(chunk& chunk, ext::data_array<chunk::vertex>& building_verts) {
    auto vert_it = building_verts.begin();
    auto vf = [&vert_it](u8 x, u8 y, u8 z, u8 u, u8 v) {
        *vert_it++ = {
            .pos = { x, y, z },
            .uv = { u, v }
        };
    };

    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &building_verts, &vert_it, &vf](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            if (get_general_vertex_count(type) != 0) {
                add_needed_face_vertices<block::face::FRONT>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::BACK>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::TOP>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::BOTTOM>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::RIGHT>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::LEFT>(chunk, vf, pos, type);
                add_general_vertices(vf, pos, type);
            } else {
                add_needed_face_vertices<block::face::FRONT>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::BACK>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::TOP>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::BOTTOM>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::RIGHT>(chunk, vf, pos, type);
                add_needed_face_vertices<block::face::LEFT>(chunk, vf, pos, type);
            }
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