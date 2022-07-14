#include "block_core.hpp"
#include "block_functionality.hpp"
#include "chunk.hpp"
#include "chunk_math.hpp"
#include "chunk_core.hpp"
#include <algorithm>

using namespace game;

bool game::does_world_position_select_block(const glm::vec3& world_pos, const block& block, const glm::vec3& world_block_pos) {
    auto offset_into_block_pos = world_pos - world_block_pos;

    return get_with_block_functionality<bool>(block.tp, [&block, &offset_into_block_pos]<typename Bf>() {
        auto boxes = Bf::get_selection_boxes(block.st);
        return std::any_of(boxes.begin(), boxes.end(), [&offset_into_block_pos](auto& box) { return box.is_inside(offset_into_block_pos); });
    });
}

std::vector<math::box> game::get_block_boxes_that_collide_with_world_box(const math::box& world_box, const block& block, const glm::vec3& world_block_pos) {
    std::vector<math::box> collided_block_boxes;
    math::box box_offset_into_block_pos = {
        world_box.lesser_corner - world_block_pos,
        world_box.greater_corner - world_block_pos
    };

    call_with_block_functionality(block.tp, [&block, &collided_block_boxes, &box_offset_into_block_pos]<typename Bf>() {
        auto block_boxes = Bf::get_collision_boxes(block.st);
        for (auto& block_box : block_boxes) {
            if (math::do_boxes_collide(box_offset_into_block_pos, block_box)) {
                collided_block_boxes.push_back(block_box);
            }
        }
    });

    return collided_block_boxes;
}

// TODO: dont use output parameters
template<block::face face>
static void set_block_lookup_neighbor(const math::vector3s32& block_pos, u16& out_face_index, bool& out_is_face_edge) {
    if (is_block_position_at_face_edge<face>(block_pos)) {
        math::vector3s32 face_block_pos = get_face_offset_position<face>(block_pos);
        face_block_pos = get_local_block_position<s32>(face_block_pos);

        out_face_index = get_index_from_position<u16>(face_block_pos);
        out_is_face_edge = true;
        return;
    }

    auto face_block_pos = get_face_offset_position<face>(block_pos);

    out_face_index = get_index_from_position<u16>(face_block_pos);
    out_is_face_edge = false;
    return;
}

void game::fill_block_neighborhood_lookups(block::neighborhood_lookups& lookups) {
    auto lookups_array = lookups.data();
    for (s32 x = 0; x < chunk::SIZE; x++) {
        for (s32 y = 0; y < chunk::SIZE; y++) {
            for (s32 z = 0; z < chunk::SIZE; z++) {
                math::vector3s32 block_pos = { x, y, z };
                auto index = get_index_from_position<std::size_t>(block_pos);
                auto& lookup = lookups_array[index];
                lookup = {
                    .position = block_pos
                };
                set_block_lookup_neighbor<block::face::FRONT>(block_pos, lookup.front_index, lookup.is_front_edge);
                set_block_lookup_neighbor<block::face::BACK>(block_pos, lookup.back_index, lookup.is_back_edge);
                set_block_lookup_neighbor<block::face::TOP>(block_pos, lookup.top_index, lookup.is_top_edge);
                set_block_lookup_neighbor<block::face::BOTTOM>(block_pos, lookup.bottom_index, lookup.is_bottom_edge);
                set_block_lookup_neighbor<block::face::RIGHT>(block_pos, lookup.right_index, lookup.is_right_edge);
                set_block_lookup_neighbor<block::face::LEFT>(block_pos, lookup.left_index, lookup.is_left_edge);
            }
        }
    }
}