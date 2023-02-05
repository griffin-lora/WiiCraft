#include "chunk_core.hpp"
#include "chunk_math.hpp"
#include "chunk_mesh_generation.hpp"
#include "common.hpp"
#include "glm/gtc/noise.hpp"
#include "block.hpp"
#include "block_util.hpp"
#include <algorithm>
using namespace game;

template<block_face_t FACE>
static chunk::opt_ref get_neighbor_from_map(chunk::map& chunks, const math::vector3s32& pos) {
    math::vector3s32 offset_pos = get_face_offset_position<FACE>(pos);
    auto it = chunks.find(offset_pos);
    if (it != chunks.end()) {
        return it->second;
    } else {
        return {};
    }
}

void game::update_chunk_neighborhood(chunk::map& chunks, const math::vector3s32& pos, chunk& chunk) {
    chunk.nh = {
        .front = get_neighbor_from_map<block_face_front>(chunks, pos),
        .back = get_neighbor_from_map<block_face_back>(chunks, pos),
        .top = get_neighbor_from_map<block_face_top>(chunks, pos),
        .bottom = get_neighbor_from_map<block_face_bottom>(chunks, pos),
        .right = get_neighbor_from_map<block_face_right>(chunks, pos),
        .left = get_neighbor_from_map<block_face_left>(chunks, pos),
    };
}

void game::add_important_chunk_mesh_update(chunk& chunk, const math::vector3s32& pos) {
    chunk.update_core_mesh_important = true;
    chunk.update_shell_mesh_important = true;
    call_func_on_each_face<void>([&chunk, &pos]<block_face_t FACE>() {
        if (is_block_position_at_face_edge<FACE>(pos)) {
            auto nb_chunk = get_neighbor<FACE>(chunk.nh);
            if (nb_chunk.has_value()) {
                nb_chunk->get().update_shell_mesh_important = true;
            }
        }
    });
}

std::size_t& game::get_block_count_ref(chunk& chunk, block_type_t type) {
    switch (type) {
        default: return chunk.fully_opaque_block_count;
        case block_type_air: return chunk.invisible_block_count;
        case block_type_tall_grass:
        case block_type_water: return chunk.partially_opaque_block_count;
    }
    // Compiler complains despite this being unreachable
    return chunk.invisible_block_count;
}

void game::add_chunk_mesh_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block_face_t FACE>() {
        auto nb_chunk = get_neighbor<FACE>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_shell_mesh_unimportant = true;
            nb_chunk->get().update_neighborhood = true;
        }
    });
}

void game::add_chunk_neighborhood_update_to_neighbors(chunk& chunk) {
    call_func_on_each_face<void>([&chunk]<block_face_t FACE>() {
        auto nb_chunk = get_neighbor<FACE>(chunk.nh);
        if (nb_chunk.has_value()) {
            nb_chunk->get().update_neighborhood = true;
        }
    });
}