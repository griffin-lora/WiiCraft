#include "block_core.hpp"
#include "block_functionality.hpp"
#include "chunk.hpp"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include <algorithm>

using namespace game;

bool game::does_world_position_select_block(const glm::vec3& world_pos, const block& block, const glm::vec3& world_block_pos) {
    auto offset_into_block_pos = world_pos - world_block_pos;

    return get_with_block_functionality<bool>(block.tp, [&block, &offset_into_block_pos]<typename BF>() {
        auto boxes = BF::get_selection_boxes(block.st);
        return std::any_of(boxes.begin(), boxes.end(), [&offset_into_block_pos](auto& box) { return box.is_inside(offset_into_block_pos); });
    });
}

std::vector<math::box> game::get_block_boxes_that_collide_with_world_box(const math::box& world_box, const block& block, const glm::vec3& world_block_pos) {
    std::vector<math::box> collided_block_boxes;
    math::box box_offset_into_block_pos = {
        world_box.lesser_corner - world_block_pos,
        world_box.greater_corner - world_block_pos
    };

    call_with_block_functionality(block.tp, [&block, &collided_block_boxes, &box_offset_into_block_pos]<typename BF>() {
        auto block_boxes = BF::get_collision_boxes(block.st);
        for (auto& block_box : block_boxes) {
            if (math::do_boxes_collide(box_offset_into_block_pos, block_box)) {
                collided_block_boxes.push_back(block_box);
            }
        }
    });

    return collided_block_boxes;
}