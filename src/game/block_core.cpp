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
        auto boxes = BF::get_selection_boxes((game::block::state)game::block::slab_state::bottom);
        return std::any_of(boxes.begin(), boxes.end(), [&offset_into_block_pos](auto& box) { return is_inside_box(box, offset_into_block_pos); });
    });
}