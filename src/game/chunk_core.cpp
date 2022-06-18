#include "chunk_core.hpp"
#include "PerlinNoise.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"

using namespace game;

void game::init(chunk& chunk, math::matrix view, const math::vector3s32& pos) {
    chunk.tf.set_position(view, pos.x * chunk::SIZE, pos.y * chunk::SIZE, pos.z * chunk::SIZE);
}

math::vector3u8 game::get_position_from_index(std::size_t index) {
    uint z = index / (chunk::SIZE * chunk::SIZE);
    index -= (z * chunk::SIZE * chunk::SIZE);
    uint y = index / chunk::SIZE * chunk::SIZE;
    uint x = index % chunk::SIZE * chunk::SIZE;
    return { x, y, z };
}

void game::generate_blocks(chunk& chunk, const math::vector3s32& chunk_pos, u32 seed) {
    siv::PerlinNoise noise(seed);
    for (u8 x = 0; x < chunk::SIZE; x++) {
        for (u8 z = 0; z < chunk::SIZE; z++) {
            auto world_x = game::get_world_coord_from_local_position(x, chunk_pos.x);
            auto world_z = game::get_world_coord_from_local_position(z, chunk_pos.z);
            auto height = noise.octave2D(world_x / 32.f, world_z / 32.f, 8);

            s32 y_pos = (height * 16);

            for (u8 y = 0; y < chunk::SIZE; y++) {
                auto world_y = game::get_world_coord_from_local_position(y, chunk_pos.y);
                auto index = game::get_index_from_position(math::vector3u8{x, y, z});

                if (world_y < y_pos) {
                    chunk.blocks[index] = { .tp = block::type::DIRT };
                } else if (world_y == y_pos) {
                    chunk.blocks[index] = { .tp = block::type::GRASS };
                } else {
                    chunk.blocks[index] = { .tp = block::type::AIR };
                }
            }
        }
    }
}

template<block::face face>
static chunk::opt_ref get_neighbor_from_map(chunk::map& chunks, const math::vector3s32& pos) {
    math::vector3s32 offset_pos = get_face_offset_position<face>(pos);
    if (chunks.count(offset_pos)) {
        return chunks.at(offset_pos);
    } else {
        return {};
    }
}

void game::update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk) {
    chunk.nh = {
        .front = get_neighbor_from_map<block::face::FRONT>(chunks, pos),
        .back = get_neighbor_from_map<block::face::BACK>(chunks, pos),
        .top = get_neighbor_from_map<block::face::TOP>(chunks, pos),
        .bottom = get_neighbor_from_map<block::face::BOTTOM>(chunks, pos),
        .right = get_neighbor_from_map<block::face::RIGHT>(chunks, pos),
        .left = get_neighbor_from_map<block::face::LEFT>(chunks, pos),
    };
}

template<block::face face>
static constexpr bool is_block_position_at_face_edge(math::vector3u8 pos) {
    static_assert(face != block::face::CENTER, "Center face is not allowed.");
    constexpr auto edge_coord = (chunk::SIZE - 1);
    return call_face_func_for<face, bool>(
        [&]() { return pos.x == edge_coord; },
        [&]() { return pos.x == 0; },
        [&]() { return pos.y == edge_coord; },
        [&]() { return pos.y == 0; },
        [&]() { return pos.z == edge_coord; },
        [&]() { return pos.z == 0; },
        []() {}
    );
}

template<block::face face>
static void add_needed_chunk_mesh_update_to_neighbor(chunk& chunk, math::vector3u8 pos) {
    if (is_block_position_at_face_edge<face>(pos)) {
        add_chunk_mesh_update_to_neighbor<face>(chunk);
    }
}

void game::add_chunk_mesh_update(chunk& chunk, math::vector3u8 pos) {
    chunk.update_mesh = true;
    add_needed_chunk_mesh_update_to_neighbor<block::face::FRONT>(chunk, pos);
    add_needed_chunk_mesh_update_to_neighbor<block::face::BACK>(chunk, pos);
    add_needed_chunk_mesh_update_to_neighbor<block::face::TOP>(chunk, pos);
    add_needed_chunk_mesh_update_to_neighbor<block::face::BOTTOM>(chunk, pos);
    add_needed_chunk_mesh_update_to_neighbor<block::face::RIGHT>(chunk, pos);
    add_needed_chunk_mesh_update_to_neighbor<block::face::LEFT>(chunk, pos);
}

template<block::face face>
static void add_chunk_mesh_neighborhood_update_to_neighbor(chunk& chunk) {
    auto nb_chunk_opt = get_neighbor<face>(chunk.nh);
    if (nb_chunk_opt.has_value()) {
        auto& nb_chunk = nb_chunk_opt->get();
        nb_chunk.update_mesh = true;
        nb_chunk.update_neighborhood = true;
    }
}

void game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk) {
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::FRONT>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::BACK>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::TOP>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::BOTTOM>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::RIGHT>(chunk);
    add_chunk_mesh_neighborhood_update_to_neighbor<block::face::LEFT>(chunk);
}

void game::update_chunks(chunk::map& chunks, ext::data_array<block::face_cache>& face_caches) {
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_neighborhood) {
            chunk.update_neighborhood = false;
            game::update_chunk_neighborhood(chunks, pos, chunk);
        }
    }
    
    for (auto& [ pos, chunk ] : chunks) {
        if (chunk.update_mesh) {
            chunk.update_mesh = false;
            game::update_mesh(chunk, face_caches);
            break;
        }
    }
}