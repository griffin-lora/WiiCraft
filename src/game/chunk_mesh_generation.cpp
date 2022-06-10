#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
constexpr const chunk* get_neighbor_for_face(const chunk_neighbors& neighbors) {
    if constexpr (face == block::face::FRONT) {
        return neighbors.front;
    } else if constexpr (face == block::face::BACK) {
        return neighbors.back;
    } else if constexpr (face == block::face::LEFT) {
        return neighbors.left;
    } else if constexpr (face == block::face::RIGHT) {
        return neighbors.right;
    } else if constexpr (face == block::face::TOP) {
        return neighbors.top;
    } else if constexpr (face == block::face::BOTTOM) {
        return neighbors.bottom;
    }
}

template<block::face face>
static bool should_render_face(const chunk& chunk, const chunk_neighbors& neighbors, block::type type, math::vector3u8 pos) {
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
        // if (get_neighbor_for_face<face>(neighbors) == nullptr) {
        //     return true;
        // } else {
        //     auto& check_chunk = *get_neighbor_for_face<face>(neighbors);
        //     math::vector3s32 greater_range_check_pos = get_face_offset_position<face, math::vector3s32>(pos);
        //     greater_range_check_pos.x = math::mod(greater_range_check_pos.x, chunk::SIZE);
        //     greater_range_check_pos.y = math::mod(greater_range_check_pos.y, chunk::SIZE);
        //     greater_range_check_pos.z = math::mod(greater_range_check_pos.z, chunk::SIZE);
        //     auto check_block_type = check_chunk.blocks[get_index_from_position(greater_range_check_pos)].tp;
        //     if (is_block_visible(check_block_type)) {
        //         return false;
        //     } else {
        //         return true;
        //     }
        // }
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
static std::size_t get_needed_face_vertex_count(const chunk& chunk, const chunk_neighbors& neighbors, block::type type, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, neighbors, type, pos)) {
        return get_face_vertex_count<face>(type);
    } else {
        return 0;
    }
}

static std::size_t get_chunk_vertex_count(const chunk& chunk, const chunk_neighbors& neighbors) {
    std::size_t vertex_count = 0;
    iterate_over_chunk_blocks_and_positions(chunk.blocks, [&chunk, &neighbors, &vertex_count](auto& block, auto pos) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, neighbors, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, neighbors, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, neighbors, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, neighbors, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, neighbors, type, pos);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, neighbors, type, pos);
        }
    });
    return vertex_count;
}

template<block::face face>
static void add_needed_face_vertices(chunk& chunk, const chunk_neighbors& neighbors, vertex_it& it, block::type type, math::vector3u8 pos) {
    if (should_render_face<face>(chunk, neighbors, type, pos)) {
        add_face_vertices_at_mut_it<face>(pos, it, type);
    }
}

void game::update_mesh(chunk& chunk, const chunk_neighbors& neighbors) {
    chunk.ms.vertices.resize_without_copying(get_chunk_vertex_count(chunk, neighbors));
    auto it = chunk.ms.vertices.begin();

    iterate_over_chunk_blocks_and_positions(chunk.blocks, [&chunk, &neighbors, &it](auto& block, auto pos) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_needed_face_vertices<block::face::FRONT>(chunk, neighbors, it, type, pos);
            add_needed_face_vertices<block::face::BACK>(chunk, neighbors, it, type, pos);
            add_needed_face_vertices<block::face::LEFT>(chunk, neighbors, it, type, pos);
            add_needed_face_vertices<block::face::RIGHT>(chunk, neighbors, it, type, pos);
            add_needed_face_vertices<block::face::TOP>(chunk, neighbors, it, type, pos);
            add_needed_face_vertices<block::face::BOTTOM>(chunk, neighbors, it, type, pos);
        }
    });

    if (it != chunk.ms.vertices.end()) {
        dbg::error([&chunk, it]() {
            printf("Vertex count mismatch! Expected %d vertices but got %d vertices\n", chunk.ms.vertices.size(), it - chunk.ms.vertices.begin());
        });
    }
}