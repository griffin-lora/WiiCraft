#pragma once
#include "world_location.h"
#include "chrono.h"
#include <ogc/gu.h>

void voxel_selection_init(void);
void voxel_selection_update(const mat4s* view);
void voxel_selection_draw(us_t now);
void voxel_selection_handle_location(const mat4s* view, world_location_t location);