#pragma once
#include "block_raycast.hpp"
#include "block_functionality.hpp"
#include "chunk_core.hpp"
#include "util.hpp"

using namespace game;

std::optional<block_raycast> game::get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& end) {
    auto dir = glm::normalize(end - origin);

    for (f32 x = origin.x; x <= end.x; x++) {
        for (f32 y = origin.y; y <= end.y; y++) {
            for (f32 z = origin.z; z <= end.z; z++) {
                glm::vec3 world_block_pos = { x, y, z };
                
                auto world_loc = get_world_location_at_world_position(chunks, world_block_pos);
                if (world_loc.has_value()) {
                    return get_with_block_functionality<std::optional<block_raycast>>(world_loc->bl.tp, [&]<typename Bf>() -> std::optional<block_raycast> {
                        auto boxes = Bf::get_selection_boxes(world_loc->bl.st);
                        for (auto& box : boxes) {
                            box.lesser_corner += world_block_pos;
                            box.greater_corner += world_block_pos;
                            auto box_raycast = math::get_box_raycast(origin, dir, box);
                            if (box_raycast.has_value()) {
                                return block_raycast{
                                    .box_raycast = *box_raycast,
                                    .location = *world_loc
                                };
                            }
                        }
                        return {};
                    });
                }

            }
        }
    }
    return {};
}