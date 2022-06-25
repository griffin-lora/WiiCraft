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

std::optional<math::box> game::get_box_that_collides_with_world_position(const glm::vec3& world_pos, const block& block, const glm::vec3& world_block_pos) {
    auto offset_into_block_pos = world_pos - world_block_pos;
    switch (block.tp) {
        default: return {};
        EVAL_BLOCK_FUNCTIONALITY_CASES(
            auto boxes = Bf::get_boxes(block.st);
            auto it = std::find_if(boxes.begin(), boxes.end(), [&offset_into_block_pos](auto& box) { return box.is_inside(offset_into_block_pos); });
            if (it != boxes.end()) {
                return *it;
            }
            return {};
        )
    }
}