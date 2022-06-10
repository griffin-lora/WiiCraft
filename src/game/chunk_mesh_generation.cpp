#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static constexpr math::vector3u8 get_face_offset_position(math::vector3u8 pos) {
    math::vector3u8 offset_pos = pos;
    if constexpr (face == block::face::FRONT) {
        offset_pos.x += 1;
    } else if constexpr (face == block::face::BACK) {
        offset_pos.x -= 1;
    } else if constexpr (face == block::face::LEFT) {
        offset_pos.z += 1;
    } else if constexpr (face == block::face::RIGHT) {
        offset_pos.z -= 1;
    } else if constexpr (face == block::face::TOP) {
        offset_pos.y += 1;
    } else if constexpr (face == block::face::BOTTOM) {
        offset_pos.y -= 1;
    }
    return offset_pos;
}

template<block::face face>
static bool should_render_face(const chunk& chunk, block::type type, math::vector3u8 pos) {
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
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
static std::size_t get_needed_face_vertex_count(const chunk& chunk, block::type type, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, type, pos)) {
        return get_face_vertex_count<face>(type);
    } else {
        return 0;
    }
}

static std::size_t get_chunk_vertex_count(const chunk& chunk) {
    std::size_t vertex_count = 0;
    iterate_over_chunk_blocks_and_positions(chunk.blocks, [&chunk, &vertex_count](auto& block, auto pos) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, type, pos);
        }
    });
    return vertex_count;
}

template<block::face face>
static void add_needed_face_vertices(chunk& chunk, vertex_it& it, block::type type, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, type, pos)) {
        add_face_vertices_at_mut_it<face>(pos, it, type);
    }
}

void game::update_mesh(chunk& chunk) {
    chunk.ms.vertices.resize_without_copying(get_chunk_vertex_count(chunk));
    auto it = chunk.ms.vertices.begin();

    iterate_over_chunk_blocks_and_positions(chunk.blocks, [&chunk, &it](auto& block, auto pos) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_needed_face_vertices<block::face::FRONT>(chunk, it, type, pos);
            add_needed_face_vertices<block::face::BACK>(chunk, it, type, pos);
            add_needed_face_vertices<block::face::LEFT>(chunk, it, type, pos);
            add_needed_face_vertices<block::face::RIGHT>(chunk, it, type, pos);
            add_needed_face_vertices<block::face::TOP>(chunk, it, type, pos);
            add_needed_face_vertices<block::face::BOTTOM>(chunk, it, type, pos);
        }
    });

    if (it != chunk.ms.vertices.end()) {
        dbg::error([]() {
            printf("Vertex count mismatch!\n");
        });
    }
}