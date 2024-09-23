#pragma once
#include "game/voxel_raycast.h"
#include <cglm/struct/mat4.h>
#include <gctypes.h>

void update_world(const mat4s* view, const voxel_raycast_t* raycast, u32 buttons_down);