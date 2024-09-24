#pragma once
#include "chrono.h"
#include "game/voxel.h"
#include "game_math.h"
#include <cglm/struct/mat4.h>
#include <ogc/gu.h>

void voxel_selection_init(void);
void voxel_selection_update_view(const mat4s* view);
void voxel_selection_draw(us_t now);
void voxel_selection_update(const mat4s* view, s32vec3s voxel_world_pos);