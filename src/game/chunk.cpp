#include "game.hpp"
#include "PerlinNoise.hpp"

using namespace game;

void game::init(chunk& chunk, const math::vector3s32& pos, math::matrix view) {
    guMtxIdentity(chunk.model);
    guMtxTransApply(chunk.model, chunk.model, pos.x * chunk::SIZE, pos.y * chunk::SIZE, pos.z * chunk::SIZE);
    update_model_view(chunk, view);
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

            s32 y_pos = (height * chunk::SIZE);

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
static chunk::const_opt_ref get_neighbor(const std::unordered_map<math::vector3s32, chunk>& chunks, const math::vector3s32& pos) {
    math::vector3s32 offset_pos = get_face_offset_position<face>(pos);
    if (chunks.count(offset_pos)) {
        return chunks.at(offset_pos);
    } else {
        return {};
    }
}

chunk_neighborhood game::get_chunk_neighborhood(const chunk::map& chunks, const math::vector3s32& pos) {
    return {
        .front = get_neighbor<block::face::FRONT>(chunks, pos),
        .back = get_neighbor<block::face::BACK>(chunks, pos),
        .right = get_neighbor<block::face::RIGHT>(chunks, pos),
        .left = get_neighbor<block::face::LEFT>(chunks, pos),
        .top = get_neighbor<block::face::TOP>(chunks, pos),
        .bottom = get_neighbor<block::face::BOTTOM>(chunks, pos),
    };
}