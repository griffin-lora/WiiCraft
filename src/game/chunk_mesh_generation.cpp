#include "chunk_mesh_generation.hpp"
#include "face_mesh_generation.hpp"
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

            auto nb_check_pos = get_face_offset_position<face, math::vector3s32>(pos);

            nb_check_pos.x = math::mod(nb_check_pos.x, chunk::SIZE);
            nb_check_pos.y = math::mod(nb_check_pos.y, chunk::SIZE);
            nb_check_pos.z = math::mod(nb_check_pos.z, chunk::SIZE);

            auto index = get_index_from_position(nb_check_pos);

            auto& block = nb_chunk.blocks[index];
            if (is_block_visible(block.tp)) {
                return false;
            }
        } else {
            // Don't render chunk faces if there is no neighbor chunk
            return false;
        }
        return true;
    }
    auto check_block_type = chunk.blocks[get_index_from_position(check_pos)].tp;
    if (is_block_visible(check_block_type)) {
        return false;
    } else {
        return true;
    }
}

template<block::face face>
static constexpr bool& get_face_cache_flag(block::face_cache& face_cache) {
    static_assert(face != block::face::CENTER, "Center face is not allowed.");
    return call_face_func_for<face, bool&>(
        [](auto& c) -> bool& { return c.front; },
        [](auto& c) -> bool& { return c.back; },
        [](auto& c) -> bool& { return c.top; },
        [](auto& c) -> bool& { return c.bottom; },
        [](auto& c) -> bool& { return c.right; },
        [](auto& c) -> bool& { return c.left; },
        []() {},
        face_cache
    );
}

template<block::face face>
static std::size_t get_needed_face_vertex_count(const chunk& chunk, ext::data_array<game::block::face_cache>& face_caches, math::vector3u8 pos, block::type type) {
    auto& face_cache_flag = get_face_cache_flag<face>(face_caches[get_index_from_position(pos)]);
    if (should_render_face<face>(chunk, pos, type)) {
        face_cache_flag = true;
        return get_face_vertex_count<face>(type);
    } else {
        face_cache_flag = false;
        return 0;
    }
}

static std::size_t get_chunk_vertex_count(const chunk& chunk, ext::data_array<game::block::face_cache>& face_caches) {
    std::size_t vertex_count = 0;
    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &face_caches, &vertex_count](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, face_caches, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, face_caches, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, face_caches, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, face_caches, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, face_caches, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, face_caches, pos, type);
        }
    });
    return vertex_count;
}

template<block::face face>
static void add_needed_face_vertices(ext::data_array<game::block::face_cache>& face_caches, ms_iters& iters, math::vector3u8 pos, block::type type) {
    if (get_face_cache_flag<face>(face_caches[get_index_from_position(pos)])) {
        add_face_vertices<face>(iters, pos, type);
    }
}

void game::update_mesh(chunk& chunk, ext::data_array<game::block::face_cache>& face_caches) {
    auto vertex_count = get_chunk_vertex_count(chunk, face_caches);

    if (vertex_count > 65535) {
        dbg::error([vertex_count]() {
            printf("Chunk vertex count is too high: %d\n", vertex_count);
        });
    }

    chunk.ms.pos_vertices.resize_without_copying_aligned(32, vertex_count);
    chunk.ms.uv_vertices.resize_without_copying_aligned(32, vertex_count);
    
    ms_iters iters = {
        .pos_it = chunk.ms.pos_vertices.begin(),
        .uv_it = chunk.ms.uv_vertices.begin()
    };

    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&face_caches, &iters](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_needed_face_vertices<block::face::FRONT>(face_caches, iters, pos, type);
            add_needed_face_vertices<block::face::BACK>(face_caches, iters, pos, type);
            add_needed_face_vertices<block::face::TOP>(face_caches, iters, pos, type);
            add_needed_face_vertices<block::face::BOTTOM>(face_caches, iters, pos, type);
            add_needed_face_vertices<block::face::RIGHT>(face_caches, iters, pos, type);
            add_needed_face_vertices<block::face::LEFT>(face_caches, iters, pos, type);
        }
    });

    if (iters.pos_it != chunk.ms.pos_vertices.end() || iters.uv_it != chunk.ms.uv_vertices.end()) {
        dbg::error([vertex_count]() {
            printf("Vertex count mismatch! Expected %d vertices\n", vertex_count);
        });
    }

    std::size_t disp_list_size = (
		1 + // GX_ClearVtxDesc
		2 * 2 + // GX_SetVtxDesc
		2 * 14 + // GX_SetVtxAttrFmt
		2 * 9 + // GX_SetArray
		4 + // GX_Begin
		vertex_count * 2 + // GX_Position1x16
		vertex_count * 2 + //GX_TexCoord1x16
		1 // GX_End
	);

    chunk.disp_list.resize(disp_list_size);

    chunk.disp_list.write_into([&chunk, vertex_count]() {
        // setup the vertex attribute table
        // describes the data
        // args: vat location 0-7, type of data, data format, size, scale
        // so for ex. in the first call we are sending position data with
        // 3 values X,Y,Z of size F32. scale sets the number of fractional
        // bits for non float data.
        GX_ClearVtxDesc();
        GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
        GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);

        // GX_VTXFMT0 is for standard cube geometry
        
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_U8, 0);
        // Since the fractional size of the fixed point number is 4, it is equivalent to 1 unit = 16 pixels
        GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 4);

        GX_SetArray(GX_VA_POS, chunk.ms.pos_vertices.data(), sizeof(chunk::mesh::pos_vertex));
        GX_SetArray(GX_VA_TEX0, chunk.ms.uv_vertices.data(), sizeof(chunk::mesh::uv_vertex));

        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);
    
        for (std::size_t i = 0; i < vertex_count; i++) {
            GX_Position1x16(i);
            GX_TexCoord1x16(i);
        }

        GX_End();
    });
}