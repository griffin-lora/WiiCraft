#pragma once
#include "chunk.hpp"
#include "block_core.hpp"
#include "chunk_math.hpp"
#include "block_mesh_generation.hpp"
#include "block_util.hpp"
#include "chrono.hpp"

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

    void add_chunk_neighborhood_update_to_neighbors(chunk& chunk);
    void add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk);
    
    std::size_t& get_block_count_ref(chunk& chunk, const block& block);

    void add_important_chunk_mesh_update(chunk& chunk, const math::vector3s32& block_position);

    struct world_location {
        math::vector3s32 ch_pos;
        math::vector3s32 bl_pos;
        chunk* ch;
        block* bl;
    };

    template<typename T>
    std::optional<world_location> get_world_location_at_world_position(chunk::map& chunks, const T& position) {
        auto chunk_pos = get_chunk_position_from_world_position(position);
        auto it = chunks.find(chunk_pos);
        if (it != chunks.end()) {
            auto& chunk = it->second;

            auto block_pos = get_local_block_position<s32>(position);
            auto index = get_index_from_position<std::size_t>(block_pos);

            auto& block = chunk.blocks[index];
            return world_location{
                .ch_pos = chunk_pos,
                .bl_pos = block_pos,
                .ch = &chunk,
                .bl = &block
            };
        }
        return {};
    }

    template<typename T>
    inline std::optional<const world_location> get_world_location_at_world_position(const chunk::map& chunks, const T& position) {
        return get_world_location_at_world_position(const_cast<chunk::map&>(chunks), position);
    }

    template<typename T>
    inline std::optional<std::reference_wrapper<block>> get_block_from_world_position(chunk::map& chunks, const T& position) {
        auto loc = get_world_location_at_world_position(chunks, position);
        if (loc.has_value()) {
            return *loc->bl;
        }
        return {};
    }

    template<typename T>
    inline std::optional<std::reference_wrapper<const block>> get_block_from_world_position(const chunk::map& chunks, const T& position) {
        return get_block_from_world_position(const_cast<chunk::map&>(chunks), position);
    }

    template<typename T>
    inline std::optional<std::reference_wrapper<chunk>> get_chunk_from_world_position(chunk::map& chunks, const T& position) {
        auto loc = get_world_location_at_world_position(chunks, position);
        if (loc.has_value()) {
            return *loc->ch;
        }
        return {};
    }

    template<typename T>
    inline std::optional<std::reference_wrapper<const chunk>> get_chunk_from_world_position(const chunk::map& chunks, const T& position) {
        return get_chunk_from_world_position(const_cast<chunk::map&>(chunks), position);
    }
}