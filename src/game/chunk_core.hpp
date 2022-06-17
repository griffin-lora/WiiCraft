#pragma once
#include "chunk.hpp"
#include "block_core.hpp"

namespace game {
    template<block::face face>
    constexpr chunk::const_opt_ref get_neighbor(const chunk::neighborhood& nh) {
        static_assert(face != block::face::CENTER, "Center face is not allowed.");
        return call_face_func_for<face, chunk::const_opt_ref>(
            [&]() { return nh.front; },
            [&]() { return nh.back; },
            [&]() { return nh.top; },
            [&]() { return nh.bottom; },
            [&]() { return nh.right; },
            [&]() { return nh.left; },
            []() {}
        );
    }

    template<block::face face>
    constexpr chunk::opt_ref get_neighbor(chunk::neighborhood& nh) {
        static_assert(face != block::face::CENTER, "Center face is not allowed.");
        return call_face_func_for<face, chunk::opt_ref>(
            [&]() { return nh.front; },
            [&]() { return nh.back; },
            [&]() { return nh.top; },
            [&]() { return nh.bottom; },
            [&]() { return nh.right; },
            [&]() { return nh.left; },
            []() {}
        );
    }

    void init(chunk& chunk, math::matrix view, const math::vector3s32& chunk_position);

    void update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk);


    math::vector3u8 get_position_from_index(std::size_t index);
    template<typename T>
    inline std::size_t get_index_from_position(T position) {
        return position.x + (position.y * chunk::SIZE) + (position.z * chunk::SIZE * chunk::SIZE);
    }
    inline s32 get_world_coord_from_local_position(s32 local_coord, s32 chunk_coord) {
        return ((chunk_coord * chunk::SIZE) + local_coord);
    }
    inline math::vector3s32 floor_float_position(const glm::vec3& position) {
        return {
            floorf(position.x),
            floorf(position.y),
            floorf(position.z)
        };
    }
    inline math::vector3s32 get_chunk_position_from_world_position(glm::vec3 world_position) {
        return floor_float_position(world_position / (f32)chunk::SIZE);
    }

    template<typename F>
    void iterate_over_chunk_positions(F func) {
        for (u8 x = 0; x < chunk::SIZE; x++) {
            for (u8 y = 0; y < chunk::SIZE; y++) {
                for (u8 z = 0; z < chunk::SIZE; z++) {
                    func({x, y, z});
                }
            }
        }
    }

    template<typename B, typename F>
    void iterate_over_chunk_blocks_and_positions(B& blocks, F func) {
        for (u8 x = 0; x < chunk::SIZE; x++) {
            for (u8 y = 0; y < chunk::SIZE; y++) {
                for (u8 z = 0; z < chunk::SIZE; z++) {
                    math::vector3u8 pos = {x, y, z};
                    func(blocks[get_index_from_position(pos)], pos);
                }
            }
        }
    }

    template<typename B, typename F>
    void iterate_over_chunk_positions_and_blocks(B& blocks, F func) {
        for (u8 x = 0; x < chunk::SIZE; x++) {
            for (u8 y = 0; y < chunk::SIZE; y++) {
                for (u8 z = 0; z < chunk::SIZE; z++) {
                    math::vector3u8 pos = {x, y, z};
                    func(pos, blocks[get_index_from_position(pos)]);
                }
            }
        }
    }

    void generate_blocks(chunk& chunk, const math::vector3s32& pos, u32 seed);

    void add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk);

    void add_chunk_mesh_update(chunk& chunk, math::vector3u8 block_position);
    template<block::face face>
    void add_chunk_mesh_update_to_neighbor(chunk& chunk) {
        auto nb_chunk_opt = get_neighbor<face>(chunk.nh);
        if (nb_chunk_opt.has_value()) {
            auto& nb_chunk = nb_chunk_opt->get();
            nb_chunk.update_mesh = true;
        }
    }
};