#include "game.hpp"

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

void game::generate_blocks(chunk& chunk) {
    iterate_over_chunk_blocks_and_positions(chunk.blocks, [](auto& block, auto pos) {
        if (pos.y <= 2) {
            block = { .tp = block::type::GRASS };
        } else {
            block = { .tp = block::type::AIR };
        }
    });
}

template<block::face face>
static inline const chunk* get_chunk_neighbor(const std::unordered_map<math::vector3s32, chunk>& chunks, math::vector3s32 pos) {
    pos = get_face_offset_position<face>(pos);
    if (chunks.count(pos) == 0) {
        return nullptr;
    } else {
        return &chunks.at(pos);
    }
}

chunk_neighbors game::get_chunk_neighbors(const std::unordered_map<math::vector3s32, chunk>& chunks, const math::vector3s32& pos) {
    return {
        .front = get_chunk_neighbor<block::face::FRONT>(chunks, pos),
        .back = get_chunk_neighbor<block::face::BACK>(chunks, pos),
        .left = get_chunk_neighbor<block::face::LEFT>(chunks, pos),
        .right = get_chunk_neighbor<block::face::RIGHT>(chunks, pos),
        .top = get_chunk_neighbor<block::face::TOP>(chunks, pos),
        .bottom = get_chunk_neighbor<block::face::BOTTOM>(chunks, pos),
    };
}