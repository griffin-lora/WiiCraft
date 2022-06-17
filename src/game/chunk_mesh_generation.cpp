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
static void add_needed_face_vertices(ext::data_array<game::block::face_cache>& face_caches, math::vector3u8 pos, block::type type) {
    if (get_face_cache_flag<face>(face_caches[get_index_from_position(pos)])) {
        add_face_vertices<face>(pos, type);
    }
}

void game::update_mesh(chunk& chunk, ext::data_array<game::block::face_cache>& face_caches) {
    auto vertex_count = get_chunk_vertex_count(chunk, face_caches);

    if (vertex_count > 65535) {
        dbg::error([vertex_count]() {
            printf("Chunk vertex count is too high: %d\n", vertex_count);
        });
    }

    std::size_t disp_list_size = (
		4 + // GX_Begin
		vertex_count * 3 + // GX_Position3u8
		vertex_count * 2 + //GX_Position2u8
		1 // GX_End
	);

    chunk.disp_list.resize(disp_list_size);

    chunk.disp_list.write_into([&chunk, &face_caches, vertex_count]() {
        GX_Begin(GX_QUADS, GX_VTXFMT0, vertex_count);

        iterate_over_chunk_positions_and_blocks(chunk.blocks, [&face_caches](auto pos, auto& block) {
            auto type = block.tp;
            if (is_block_visible(type)) {
                add_needed_face_vertices<block::face::FRONT>(face_caches, pos, type);
                add_needed_face_vertices<block::face::BACK>(face_caches, pos, type);
                add_needed_face_vertices<block::face::TOP>(face_caches, pos, type);
                add_needed_face_vertices<block::face::BOTTOM>(face_caches, pos, type);
                add_needed_face_vertices<block::face::RIGHT>(face_caches, pos, type);
                add_needed_face_vertices<block::face::LEFT>(face_caches, pos, type);
            }
        });

        GX_End();
    });
}