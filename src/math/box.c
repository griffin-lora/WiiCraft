#include "box.h"

bool is_inside_box(box_t box, vec3s pos) {
    return pos.x >= box.lesser_corner.x && pos.y >= box.lesser_corner.y && pos.z >= box.lesser_corner.z && pos.x <= box.greater_corner.x && pos.y <= box.greater_corner.y && pos.z <= box.greater_corner.z;
}

bool do_boxes_collide(box_t box_a, box_t box_b) {
    return
        (box_a.lesser_corner.x <= box_b.greater_corner.x && box_a.greater_corner.x >= box_b.lesser_corner.x) &&
        (box_a.lesser_corner.y <= box_b.greater_corner.y && box_a.greater_corner.y >= box_b.lesser_corner.y) &&
        (box_a.lesser_corner.z <= box_b.greater_corner.z && box_a.greater_corner.z >= box_b.lesser_corner.z);
}