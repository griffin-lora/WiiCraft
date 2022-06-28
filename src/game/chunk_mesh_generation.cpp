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

void game::update_mesh(chunk& chunk, ext::data_array<chunk::vertex>& building_verts) {
    struct {
        ext::data_array<game::chunk::vertex>::iterator vert_it;
        std::size_t standard_vert_count;
        std::size_t foliage_vert_count;

        inline void add(u8 x, u8 y, u8 z, u8 u, u8 v) {
            standard_vert_count++;
            *vert_it++ = {
                .tp = chunk::vertex::type::STANDARD,
                .pos = { x, y, z },
                .uv = { u, v }
            };
        }

        inline void add_foliage(u8 x, u8 y, u8 z, u8 u, u8 v) {
            foliage_vert_count++;
            *vert_it++ = {
                .tp = chunk::vertex::type::FOLIAGE,
                .pos = { x, y, z },
                .uv = { u, v }
            };
        }
    } vf = {
        .vert_it = building_verts.begin()
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

                std::size_t total_vert_count = vf.vert_it - building_verts.begin();
                if (total_vert_count > chunk::MAX_VERTEX_COUNT) {
                    dbg::error([total_vert_count]() {
                        printf("Chunk vertex count is too high: %d\n", total_vert_count);
                    });
                }
            }
        }
    }

    std::size_t total_vert_count = vf.vert_it - building_verts.begin();

    std::size_t disp_list_size = (
		4 + // GX_Begin
		total_vert_count * 3 + // GX_Position3u8
		total_vert_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    chunk.disp_list.resize(disp_list_size);

    chunk.disp_list.write_into([&building_verts, &vf]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vf.standard_vert_count);

        for (auto it = building_verts.begin(); it != vf.vert_it; ++it) {
            if (it->tp == chunk::vertex::type::STANDARD) {
                GX_Position3u8(it->pos.x, it->pos.y, it->pos.z);
                GX_TexCoord2u8(it->uv.x, it->uv.y);
            }
        }
        
        GX_End();

        // TODO: Possibly optimize this by having the above iteration overwrite the previous vertices with foliage vertices so that we can iterate over less vertices.
        GX_Begin(GX_QUADS, GX_VTXFMT0, vf.foliage_vert_count);

        for (auto it = building_verts.begin(); it != vf.vert_it; ++it) {
            if (it->tp == chunk::vertex::type::FOLIAGE) {
                GX_Position3u8(it->pos.x, it->pos.y, it->pos.z);
                GX_TexCoord2u8(it->uv.x, it->uv.y);
            }
        }
        
        GX_End();
    });
}