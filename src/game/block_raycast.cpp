#include "block_raycast.hpp"
#include "chunk_core.hpp"
#include "util.hpp"
#include "block.hpp"

using namespace game;

static block_raycast_wrap_t get_closest_raycast(block_raycast_wrap_t closest_raycast, glm::vec3 world_block_pos, std::optional<world_location>& world_loc, box_raycast_wrap_t& box_raycast) {
    if (
        !box_raycast.success ||
        box_raycast.val.near_hit_time >= 1.0f ||
        (closest_raycast.success && box_raycast.val.near_hit_time >= closest_raycast.val.box_raycast.near_hit_time)
    ) {
        return closest_raycast;
    }
    return block_raycast_wrap_t{
        .success = true,
        .val = {
            .location = *world_loc,
            .box_raycast = box_raycast.val,
            .world_block_position = world_block_pos
        }
    };
}

static box_raycast_wrap_t get_box_raycast_for_block(glm::vec3 origin, glm::vec3 dir, glm::vec3 dir_inv, glm::vec3 box_transform, block_box_type_t box_type, glm::vec3 world_block_pos, block_type_t block_type) {
    box_t box;
    switch (block_type) {
        case block_type_air: return box_raycast_wrap_t{ false };
        case block_type_tall_grass:
            if (box_type == block_box_type_collision) {
                return box_raycast_wrap_t{ false };
            }
            box = {
                { 0.2f, 0.0f, 0.2f },
                { 0.8f, 0.8f, 0.8f }
            }; break;
        case block_type_water:
            if (box_type == block_box_type_collision) {
                return box_raycast_wrap_t{ false };
            }
        default:
            box = {
                { 0.0f, 0.0f, 0.0f },
                { 1.0f, 1.0f, 1.0f }
            }; break;
    }

    box.lesser_corner -= box_transform;
    box.greater_corner += box_transform;
    box.lesser_corner += world_block_pos;
    box.greater_corner += world_block_pos;

    return get_box_raycast(origin, dir, dir_inv, box);
}

block_raycast_wrap_t get_block_raycast(chunk::map& chunks, glm::vec3 origin, glm::vec3 dir, glm::vec3 begin, glm::vec3 end, glm::vec3 box_transform, block_box_type_t box_type) {
    block_raycast_wrap_t closest_raycast = { false };

    auto dir_inv = 1.0f / dir;

    auto floored_begin = floor_float_position<f32>(begin);
    auto floored_end = floor_float_position<f32>(end);

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
                    box_raycast_wrap_t box_raycast = get_box_raycast_for_block(origin, dir, dir_inv, box_transform, box_type, world_block_pos, *world_loc->bl_tp);
                    closest_raycast = get_closest_raycast(closest_raycast, world_block_pos, world_loc, box_raycast);
                }
            }
        }
    }

    return closest_raycast;
}