#pragma once
#include "block_world_core.h"
#include "chrono.h"
#include <ogc/gu.h>

void block_selection_init(void);
void block_selection_update(const mat4s* view);
void block_selection_draw(us_t now);
void block_selection_handle_location(const mat4s* view, world_location_t location);