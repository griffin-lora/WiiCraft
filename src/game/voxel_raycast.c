#include "voxel_raycast.h"
#include "world_location.h"
#include "util.h"
#include "voxel.h"

static voxel_raycast_wrap_t get_closest_raycast(voxel_raycast_wrap_t closest_raycast, world_location_t world_loc, box_raycast_wrap_t box_raycast) {
    if (
        !box_raycast.success ||
        box_raycast.val.near_hit_time >= 1.0f ||
        (closest_raycast.success && box_raycast.val.near_hit_time >= closest_raycast.val.box_raycast.near_hit_time)
    ) {
        return closest_raycast;
    }
    return (voxel_raycast_wrap_t) {
        .success = true,
        .val = {
            .location = world_loc,
            .box_raycast = box_raycast.val,
        }
    };
}

static box_raycast_wrap_t get_box_raycast_for_voxel(vec3s origin, vec3s dir, vec3s dir_inv, vec3s box_transform, voxel_box_type_t box_type, vec3s voxel_world_pos, voxel_type_t voxel_type) {
    box_t box;
    switch (voxel_type) {
        case voxel_type_air: return (box_raycast_wrap_t) { .success = false };
        case voxel_type_tall_grass:
            if (box_type == voxel_box_type_collision) {
                return (box_raycast_wrap_t) { .success = false };
            }
            box = (box_t) {
                { .x = 0.2f, .y = 0.0f, .z = 0.2f },
                { .x = 0.8f, .y = 0.8f, .z = 0.8f }
            }; break;
        case voxel_type_water:
            if (box_type == voxel_box_type_collision) {
                return (box_raycast_wrap_t) { .success = false };
            }
            box = (box_t) {
                { .x = 0.0f, .y = 0.0f, .z = 0.0f },
                { .x = 1.0f, .y = 1.0f, .z = 1.0f }
            }; break;
        default:
            box = (box_t) {
                { .x = 0.0f, .y = 0.0f, .z = 0.0f },
                { .x = 1.0f, .y = 1.0f, .z = 1.0f }
            }; break;
    }

    box.lesser_corner = glms_vec3_add(glms_vec3_sub(box.lesser_corner, box_transform), voxel_world_pos);
    box.greater_corner = glms_vec3_add(glms_vec3_add(box.greater_corner, box_transform), voxel_world_pos);

    return get_box_raycast(origin, dir, dir_inv, box);
}

voxel_raycast_wrap_t get_voxel_raycast(s32vec3s region_pos, vec3s origin, vec3s dir, vec3s begin, vec3s end, vec3s box_transform, voxel_box_type_t box_type) {
    voxel_raycast_wrap_t closest_raycast = { .success = false };

    vec3s dir_inv = glms_vec3_div((vec3s){ .x = 1.0f, .y = 1.0f, .z = 1.0f }, dir);

    vec3s floored_begin = { .x = floorf(begin.x), .y = floorf(begin.y), .z = floorf(begin.z) };
    vec3s floored_end = { .x = floorf(end.x), .y = floorf(end.y), .z = floorf(end.z) };

    // Swap floored_begin and floored_end to make sure that floored_begin is always before floored_end
    f32 temp;
    if (floored_begin.x > floored_end.x) {
        temp = floored_begin.x;
        floored_begin.x = floored_end.x;
        floored_end.x = temp;
    }
    if (floored_begin.y > floored_end.y) {
        temp = floored_begin.y;
        floored_begin.y = floored_end.y;
        floored_end.y = temp;
    }
    if (floored_begin.z > floored_end.z) {
        temp = floored_begin.z;
        floored_begin.z = floored_end.z;
        floored_end.z = temp;
    }

    for (f32 x = floored_begin.x; x <= floored_end.x; x++) {
        for (f32 y = floored_begin.y; y <= floored_end.y; y++) {
            for (f32 z = floored_begin.z; z <= floored_end.z; z++) {
                (void)dir_inv;
                (void)region_pos;
                (void)origin;
                (void)box_transform;
                (void)box_type;
                (void)get_closest_raycast;
                (void)get_box_raycast_for_voxel;
                // vec3s voxel_world_pos = { .x = x, .y = y, .z = z };
                
                // world_location_wrap_t world_loc = get_world_location_at_voxel_world_position(region_pos, voxel_world_pos);
                // if (world_loc.success) {
                //     box_raycast_wrap_t box_raycast = get_box_raycast_for_voxel(origin, dir, dir_inv, box_transform, box_type, voxel_world_pos, *world_loc.val.voxel_type);
                //     closest_raycast = get_closest_raycast(closest_raycast, world_loc.val, box_raycast);
                // }
            }
        }
    }

    return closest_raycast;
}