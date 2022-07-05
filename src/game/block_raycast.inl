#pragma once
#include "block_raycast.hpp"
#include "block_functionality.hpp"
#include "chunk_core.hpp"
#include "util.hpp"

using namespace game;

template<typename F>
std::optional<block_raycast> game::get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& end, F get_boxes) {
    auto dir = glm::normalize(end - origin);

    auto floored_origin = floor_float_position<glm::vec3>(origin);
    auto floored_end = floor_float_position<glm::vec3>(end);

    // Swap floored_origin and floored_end to make sure that floored_origin is always before floored_end
    if (floored_origin.x > floored_end.x) {
        std::swap(floored_origin.x, floored_end.x);
    }
    if (floored_origin.y > floored_end.y) {
        std::swap(floored_origin.y, floored_end.y);
    }
    if (floored_origin.z > floored_end.z) {
        std::swap(floored_origin.z, floored_end.z);
    }

    std::optional<block_raycast> closest_raycast;

    for (f32 x = floored_origin.x; x <= floored_end.x; x++) {
        for (f32 y = floored_origin.y; y <= floored_end.y; y++) {
            for (f32 z = floored_origin.z; z <= floored_end.z; z++) {
                glm::vec3 world_block_pos = { x, y, z };
                
                auto world_loc = get_world_location_at_world_position(chunks, world_block_pos);
                if (world_loc.has_value()) {
                    auto box_raycast = get_with_block_functionality<std::optional<math::box_raycast>>(world_loc->bl->tp, [&]<typename Bf>() -> std::optional<math::box_raycast> {

                        auto boxes = get_boxes.template operator()<Bf>(world_loc->bl->st);
                        for (auto& box : boxes) {
                            box.lesser_corner += world_block_pos;
                            box.greater_corner += world_block_pos;

                            return math::get_box_raycast(origin, dir, box);

                        }

                        return {};
                    });
                    if (box_raycast.has_value()) {
                        if (closest_raycast.has_value()) {
                            if (glm::length(closest_raycast->box_raycast.intersection_position - origin) > glm::length(box_raycast->intersection_position - origin)) {
                                closest_raycast = block_raycast{
                                    .box_raycast = *box_raycast,
                                    .location = *world_loc
                                };
                            }
                        } else {
                            closest_raycast = block_raycast{
                                .box_raycast = *box_raycast,
                                .location = *world_loc
                            };
                        }
                    }
                }

            }
        }
    }
    return closest_raycast;
}