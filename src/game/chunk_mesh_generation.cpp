#include "game.hpp"
#include "dbg.hpp"
#include <cstdio>

using namespace game;

template<block::face face>
static chunk::opt_ref get_neighbor(const chunk::neighborhood& nh) {
    if constexpr (face == block::face::FRONT) {
        return nh.front;
    } else if constexpr (face == block::face::BACK) {
        return nh.back;
    } else if constexpr (face == block::face::RIGHT) {
        return nh.right;
    } else if constexpr (face == block::face::LEFT) {
        return nh.left;
    } else if constexpr (face == block::face::TOP) {
        return nh.top;
    } else if constexpr (face == block::face::BOTTOM) {
        return nh.bottom;
    }
}

template<block::face face>
static bool should_render_face(const chunk& chunk, math::vector3u8 pos, block::type type) {
    auto check_pos = get_face_offset_position<face>(pos);
    if (check_pos.x >= chunk::SIZE || check_pos.y >= chunk::SIZE || check_pos.z >= chunk::SIZE) {
        // Since the position is outside of the chunk, we need to check the neighbor chunk
        auto nb_chunk_opt = get_neighbor<face>(chunk.nh);

        if (nb_chunk_opt.has_value()) {
            auto& nb_chunk = nb_chunk_opt.value().get();

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
        if (is_block_visible(type)) {
            vertex_count += get_needed_face_vertex_count<block::face::FRONT>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BACK>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::RIGHT>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::LEFT>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::TOP>(chunk, pos, type);
            vertex_count += get_needed_face_vertex_count<block::face::BOTTOM>(chunk, pos, type);
        }
    });
    return vertex_count;
}

template<block::face face>
static void add_needed_face_vertices(chunk& chunk, vertex_it& it, math::vector3u8 pos, block::type type) {
    if (should_render_face<face>(chunk, pos, type)) {
        add_face_vertices<face>(it, pos, type);
    }
}

void game::update_mesh(chunk& chunk) {
    chunk.ms.vertices.resize_without_copying(get_chunk_vertex_count(chunk));
    auto it = chunk.ms.vertices.begin();

    iterate_over_chunk_positions_and_blocks(chunk.blocks, [&chunk, &it](auto pos, auto& block) {
        auto type = block.tp;
        if (is_block_visible(type)) {
            add_needed_face_vertices<block::face::FRONT>(chunk, it, pos, type);
            add_needed_face_vertices<block::face::BACK>(chunk, it, pos, type);
            add_needed_face_vertices<block::face::RIGHT>(chunk, it, pos, type);
            add_needed_face_vertices<block::face::LEFT>(chunk, it, pos, type);
            add_needed_face_vertices<block::face::TOP>(chunk, it, pos, type);
            add_needed_face_vertices<block::face::BOTTOM>(chunk, it, pos, type);
        }
    });

    if (it != chunk.ms.vertices.end()) {
        dbg::error([&chunk, it]() {
            printf("Vertex count mismatch! Expected %d vertices but got %d vertices\n", chunk.ms.vertices.size(), it - chunk.ms.vertices.begin());
        });
    }
}

template<block::face face>
static void update_neighbor_mesh(chunk& chunk) {
    auto nb_chunk_opt = get_neighbor<face>(chunk.nh);
    if (nb_chunk_opt.has_value()) {
        auto& nb_chunk = nb_chunk_opt->get();
        update_mesh(nb_chunk);
    }
}

void game::update_mesh_and_neighborhood_meshes(chunk& chunk) {
    update_mesh(chunk);
    update_neighbor_mesh<block::face::FRONT>(chunk);
    update_neighbor_mesh<block::face::BACK>(chunk);
    update_neighbor_mesh<block::face::RIGHT>(chunk);
    update_neighbor_mesh<block::face::LEFT>(chunk);
    update_neighbor_mesh<block::face::TOP>(chunk);
    update_neighbor_mesh<block::face::BOTTOM>(chunk);
}