#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static chunk_neighborhood::chunk_ref_opt get_chunk_ref_opt_from_nb(const chunk_neighborhood& nb) {
    if constexpr (face == block::face::FRONT) {
        return nb.front;
    } else if constexpr (face == block::face::BACK) {
        return nb.back;
    } else if constexpr (face == block::face::RIGHT) {
        return nb.right;
    } else if constexpr (face == block::face::LEFT) {
        return nb.left;
    } else if constexpr (face == block::face::TOP) {
        return nb.top;
    } else if constexpr (face == block::face::BOTTOM) {
        return nb.bottom;
    }
}

template<block::face face>
static bool should_render_face(const chunk& chunk, const chunk_neighborhood& nb, math::vector3u8 pos, block::type type) {
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
        // Since the position is outside of the chunk, we need to check the neighbor chunk
        auto chunk_ref_opt = get_chunk_ref_opt_from_nb<face>(nb);

        if (chunk_ref_opt.has_value()) {
            auto& chunk_ref = chunk_ref_opt.value().get();

            auto nb_check_pos = get_face_offset_position<face, math::vector3s32>(pos);

            nb_check_pos.x = math::mod(nb_check_pos.x, chunk::SIZE);
            nb_check_pos.y = math::mod(nb_check_pos.y, chunk::SIZE);
            nb_check_pos.z = math::mod(nb_check_pos.z, chunk::SIZE);

            auto index = get_index_from_position(nb_check_pos);

            auto& block = chunk_ref.blocks[index];
            if (is_block_visible(block.tp)) {
                return false;
            }
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
static std::size_t get_needed_face_vertex_count(const chunk& chunk, const chunk_neighborhood& nb, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, nb, pos, type)) {
        return get_face_vertex_count<face>(type);
    } else {
        return 0;
    }
}

static std::size_t get_chunk_vertex_count(const chunk& chunk, const chunk_neighborhood& nb) {
    std::size_t vertex_count = 0;
    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &nb, &vertex_count](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, nb, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, nb, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, nb, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, nb, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, nb, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, nb, pos, type);
        }
    });
    return vertex_count;
}

template<block::face face>
static void add_needed_face_vertices(chunk& chunk, const chunk_neighborhood& nb, vertex_it& it, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, nb, pos, type)) {
        add_face_vertices<face>(it, pos, type);
    }
}

void game::update_mesh(chunk& chunk, const chunk_neighborhood& nb) {
    chunk.ms.vertices.resize_without_copying(get_chunk_vertex_count(chunk, nb));
    auto it = chunk.ms.vertices.begin();

    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &nb, &it](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_needed_face_vertices<block::face::FRONT>(chunk, nb, it, pos, type);
            add_needed_face_vertices<block::face::BACK>(chunk, nb, it, pos, type);
            add_needed_face_vertices<block::face::RIGHT>(chunk, nb, it, pos, type);
            add_needed_face_vertices<block::face::LEFT>(chunk, nb, it, pos, type);
            add_needed_face_vertices<block::face::TOP>(chunk, nb, it, pos, type);
            add_needed_face_vertices<block::face::BOTTOM>(chunk, nb, it, pos, type);
        }
    });

    if (it != chunk.ms.vertices.end()) {
        dbg::error([&chunk, it]() {
            printf("Vertex count mismatch! Expected %d vertices but got %d vertices\n", chunk.ms.vertices.size(), it - chunk.ms.vertices.begin());
        });
    }
}