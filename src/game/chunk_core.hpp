#pragma once
#include "chunk.hpp"
#include "block_core.hpp"
#include "chunk_math.hpp"

namespace game {
    template<block::face face>
    constexpr chunk::const_opt_ref get_neighbor(const chunk::neighborhood& nh) {
        return call_face_func_for<face, chunk::const_opt_ref>(
            [&]() { return nh.front; },
            [&]() { return nh.back; },
            [&]() { return nh.top; },
            [&]() { return nh.bottom; },
            [&]() { return nh.right; },
            [&]() { return nh.left; }
        );
    }

    template<block::face face>
    constexpr chunk::opt_ref get_neighbor(chunk::neighborhood& nh) {
        return call_face_func_for<face, chunk::opt_ref>(
            [&]() { return nh.front; },
            [&]() { return nh.back; },
            [&]() { return nh.top; },
            [&]() { return nh.bottom; },
            [&]() { return nh.right; },
            [&]() { return nh.left; }
        );
    }

    void update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk);

    template<typename F>
    inline void iterate_over_chunk_positions(F func) {
        for (u8 x = 0; x < chunk::SIZE; x++) {
            for (u8 y = 0; y < chunk::SIZE; y++) {
                for (u8 z = 0; z < chunk::SIZE; z++) {
                    func({x, y, z});
                }
            }
        }
    }

    template<typename B, typename F>
    inline void iterate_over_chunk_blocks_and_positions(B& blocks, F func) {
        for (u8 x = 0; x < chunk::SIZE; x++) {
            for (u8 y = 0; y < chunk::SIZE; y++) {
                for (u8 z = 0; z < chunk::SIZE; z++) {
                    math::vector3u8 pos = {x, y, z};
                    func(blocks[get_index_from_position(pos)], pos);
                }
            }
        }
    }

    template<typename T, typename B, typename F>
    inline void iterate_over_chunk_positions_and_blocks(B& blocks, F func) {
        for (T x = 0; x < (T)chunk::SIZE; x++) {
            for (T y = 0; y < (T)chunk::SIZE; y++) {
                for (T z = 0; z < (T)chunk::SIZE; z++) {
                    glm::vec<3, T, glm::defaultp> pos = {x, y, z};
                    func(pos, blocks[get_index_from_position(pos)]);
                }
            }
        }
    }

    template<block::face face, typename T>
    constexpr bool is_block_position_at_face_edge(T pos) {
        constexpr auto edge_coord = (chunk::SIZE - 1);
        return call_face_func_for<face, bool>(
            [&]() { return pos.x == edge_coord; },
            [&]() { return pos.x == 0; },
            [&]() { return pos.y == edge_coord; },
            [&]() { return pos.y == 0; },
            [&]() { return pos.z == edge_coord; },
            [&]() { return pos.z == 0; }
        );
    }

    void generate_blocks(chunk& chunk, const math::vector3s32& pos);

    void add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk);

    void add_important_chunk_mesh_update(chunk& chunk, math::vector3u8 block_position);
    template<block::face face>
    void add_important_chunk_mesh_update_to_neighbor(chunk& chunk) {
        auto nb_chunk_opt = get_neighbor<face>(chunk.nh);
        if (nb_chunk_opt.has_value()) {
            auto& nb_chunk = nb_chunk_opt->get();
            nb_chunk.update_mesh_important = true;
        }
    }

    void update_chunks(chunk::map& chunks, ext::data_array<chunk::vertex>& building_vertices);

    template<typename T>
    std::optional<std::reference_wrapper<block>> get_block_from_world_position(chunk::map& chunks, const T& position) {
        auto chunk_pos = get_chunk_position_from_world_position(position);
        if (chunks.count(chunk_pos)) {
            auto& chunk = chunks.at(chunk_pos);

            auto block_pos = get_local_block_position(position);
            auto index = get_index_from_position(block_pos);

            auto& block = chunk.blocks[index];
            return std::reference_wrapper{ block };
        }
        return {};
    }
}