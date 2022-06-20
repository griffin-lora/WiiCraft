#include "chunk_mesh_generation.hpp"
#include "face_mesh_generation.hpp"
#include "face_mesh_generation_core.hpp"
#include "face_mesh_generation_core.inl"
#include "chunk_math.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static bool should_render_face(const chunk& chunk, math::vector3u8 pos, block::type type) {
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
        // Since the position is outside of the chunk, we need to check the neighbor chunk
        auto nb_chunk_opt = get_neighbor<face>(chunk.nh);

        if (nb_chunk_opt.has_value()) {
            auto& nb_chunk = nb_chunk_opt->get();

            auto nb_check_pos = get_local_block_position(get_face_offset_position<face, math::vector3s32>(pos));

            auto index = get_index_from_position(nb_check_pos);

            auto& block = nb_chunk.blocks[index];
            if (is_block_solid(block.tp)) {
                return false;
            }
        } else {
            // Don't render chunk faces if there is no neighbor chunk
            return false;
        }
        return true;
    }
    auto check_block_type = chunk.blocks[get_index_from_position(check_pos)].tp;
    if (is_block_solid(check_block_type)) {
        return false;
    } else {
        return true;
    }
}

template<block::face face>
static std::size_t get_needed_face_vertex_count(const chunk& chunk, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, pos, type)) {
        return get_face_vertex_count<face>(type);
    } else {
        return 0;
    }
}

static std::size_t get_chunk_vertex_count(const chunk& chunk) {
    std::size_t vertex_count = 0;
    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &vertex_count](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_solid(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, pos, type);
        }
    });
    return vertex_count;
}
struct chunk_mesh_vert_func {
    inline void operator()(u8 x, u8 y, u8 z, u8 u, u8 v) {
        GX_Position3u8(x, y, z);
        GX_TexCoord2u8(u, v);
    }
};

template<block::face face>
static void add_needed_face_vertices(const chunk& chunk, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, pos, type)) {
        chunk_mesh_vert_func vf;
        add_face_vertices<face>(vf, pos, type);
    }
}

void game::update_mesh(chunk& chunk, ext::data_array<chunk::vertex>& building_verts) {
    auto vertex_count = get_chunk_vertex_count(chunk);

    if (vertex_count > 65535) {
        dbg::error([vertex_count]() {
            printf("Chunk vertex count is too high: %d\n", vertex_count);
        });
    }

    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 3 + // GX_Position3u8
		vertex_count * 2 + // GX_TexCoord2u8
		1 // GX_End
	);

    chunk.disp_list.resize(disp_list_size);

    chunk.disp_list.write_into([&chunk, &building_verts, vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &building_verts](auto pos, auto& block) {
            auto type = block.tp;
            if (is_block_solid(type)) {
                add_needed_face_vertices<block::face::FRONT>(chunk, pos, type);
                add_needed_face_vertices<block::face::BACK>(chunk, pos, type);
                add_needed_face_vertices<block::face::TOP>(chunk, pos, type);
                add_needed_face_vertices<block::face::BOTTOM>(chunk, pos, type);
                add_needed_face_vertices<block::face::RIGHT>(chunk, pos, type);
                add_needed_face_vertices<block::face::LEFT>(chunk, pos, type);
            }
        });

        GX_End();
    });
}