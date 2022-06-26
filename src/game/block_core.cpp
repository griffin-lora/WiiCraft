#include "block_core.hpp"
#include "block_functionality.hpp"
#include <algorithm>

using namespace game;

bool game::does_world_position_collide_with_block(const glm::vec3& world_pos, const block& block, const glm::vec3& world_block_pos) {
    auto offset_into_block_pos = world_pos - world_block_pos;
    switch (block.tp) {
        default: return false;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            auto boxes = Bf::get_boxes(block.st);
            return std::any_of(boxes.begin(), boxes.end(), [&offset_into_block_pos](auto& box) { return box.is_inside(offset_into_block_pos); });
        )
    }
}

std::vector<math::box> game::get_block_boxes_that_collides_with_world_box(const math::box& world_box, const block& block, const glm::vec3& world_block_pos) {
    std::vector<math::box> collided_block_boxes;
    math::box box_offset_into_block_pos = {
        .lesser_corner = world_box.lesser_corner - world_block_pos,
        .greater_corner = world_box.greater_corner - world_block_pos
    };
    switch (block.tp) {
        default: break;
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            auto block_boxes = Bf::get_boxes(block.st);
            for (auto& block_box : block_boxes) {
                if (math::do_boxes_collide(box_offset_into_block_pos, block_box)) {
                    collided_block_boxes.push_back(block_box);
                }
            }
            break;
        )
    }
    return collided_block_boxes;
}