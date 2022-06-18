#include "face_mesh_generation.hpp"

using namespace game;

std::size_t game::get_center_vertex_count(block::type type) {
    return 0;
}

std::size_t game::get_any_face_vertex_count(block::type type) {
    switch (type) {
        default:
        case block::type::AIR:
            return 0;
        case block::type::DEBUG:
        case block::type::GRASS:
        case block::type::DIRT:
        case block::type::WOOD_PLANKS:
            return 4;
    }
}

std::size_t game::get_block_vertex_count(block::type type) {
    return
        get_face_vertex_count<block::face::FRONT>(type) +
        get_face_vertex_count<block::face::BACK>(type) +
        get_face_vertex_count<block::face::TOP>(type) +
        get_face_vertex_count<block::face::BOTTOM>(type) +
        get_face_vertex_count<block::face::RIGHT>(type) +
        get_face_vertex_count<block::face::LEFT>(type);
}