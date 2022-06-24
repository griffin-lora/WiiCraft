#include "face_mesh_generation.hpp"

using namespace game;

std::size_t game::get_block_vertex_count(const block& block) {
    switch (block.tp) {
        default: return 0;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            return
                Bf::get_face_traits<block::face::FRONT>(block.st).vertex_count +
                Bf::get_face_traits<block::face::BACK>(block.st).vertex_count +
                Bf::get_face_traits<block::face::TOP>(block.st).vertex_count +
                Bf::get_face_traits<block::face::BOTTOM>(block.st).vertex_count +
                Bf::get_face_traits<block::face::RIGHT>(block.st).vertex_count +
                Bf::get_face_traits<block::face::LEFT>(block.st).vertex_count +
                Bf::get_block_traits(block.st).general_vertex_count;
        )
    }
}