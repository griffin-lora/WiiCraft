#pragma once
#include "block_raycast.hpp"
#include "block_functionality.hpp"
#include "chunk_core.hpp"
#include "util.hpp"

using namespace game;

template<typename F1, typename F2, typename F3>
void game::handle_block_raycasts_in_region(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box, F3 handle_box_raycast) {
    auto dir_inv = 1.0f / dir;

    auto floored_begin = floor_float_position<glm::vec3>(begin);
    auto floored_end = floor_float_position<glm::vec3>(end);

    // Swap floored_begin and floored_end to make sure that floored_begin is always before floored_end
    if (floored_begin.x > floored_end.x) {
        std::swap(floored_begin.x, floored_end.x);
    }
    if (floored_begin.y > floored_end.y) {
        std::swap(floored_begin.y, floored_end.y);
    }
    if (floored_begin.z > floored_end.z) {
        std::swap(floored_begin.z, floored_end.z);
    }

    for (f32 x = floored_begin.x; x <= floored_end.x; x++) {
        for (f32 y = floored_begin.y; y <= floored_end.y; y++) {
            for (f32 z = floored_begin.z; z <= floored_end.z; z++) {
                const glm::vec3 world_block_pos = { x, y, z };
                
                auto world_loc = get_world_location_at_world_position(chunks, world_block_pos);
                if (world_loc.has_value()) {
                    auto box_raycast = get_with_block_functionality<std::optional<math::box_raycast>>(world_loc->bl->tp, [&]<typename Bf>() -> std::optional<math::box_raycast> {

                        auto boxes = get_boxes.template operator()<Bf>(world_loc->bl->st);
                        for (auto& box : boxes) {
                            transform_box(box);
                            box.lesser_corner += world_block_pos;
                            box.greater_corner += world_block_pos;

                            return math::get_box_raycast(origin, dir, dir_inv, box);
                        }

                        return {};
                    });
                    handle_box_raycast(world_block_pos, world_loc, box_raycast);
                }
            }
        }
    }
}

template<typename F1, typename F2>
std::optional<block_raycast> game::get_block_raycast(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box) {
    std::optional<block_raycast> closest_raycast;

    handle_block_raycasts_in_region(chunks, origin, dir, begin, end, get_boxes, transform_box, [&closest_raycast](const auto& world_block_pos, auto& world_loc, auto& box_raycast) {
        if (box_raycast.has_value() && box_raycast->near_hit_time < 1.0f) {
            if (closest_raycast.has_value()) {
                if (box_raycast->near_hit_time < closest_raycast->box_raycast.near_hit_time) {
                    closest_raycast = block_raycast{
                        .location = *world_loc,
                        .box_raycast = *box_raycast,
                        .world_block_position = world_block_pos
                    };
                }
            } else {
                closest_raycast = block_raycast{
                    .location = *world_loc,
                    .box_raycast = *box_raycast,
                    .world_block_position = world_block_pos
                };
            }
        }
    });
    
    return closest_raycast;
}

template<typename F1, typename F2>
std::vector<block_raycast> game::get_block_raycasts(chunk::map& chunks, const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& begin, const glm::vec3& end, F1 get_boxes, F2 transform_box) {
    std::vector<block_raycast> raycasts;

    handle_block_raycasts_in_region(chunks, origin, dir, begin, end, get_boxes, transform_box, [&raycasts](const auto& world_block_pos, auto& world_loc, auto& box_raycast) {
        if (box_raycast.has_value() && box_raycast->near_hit_time < 1.0f) {
            raycasts.push_back({
                .box_raycast = *box_raycast,
                .location = *world_loc,
                .world_block_position = world_block_pos
            });
        }
    });

    std::sort(raycasts.begin(), raycasts.end(), [](const auto& a, const auto& b) {
        return a.box_raycast.near_hit_time < b.box_raycast.near_hit_time;
    });


    return raycasts;
}