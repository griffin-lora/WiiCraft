#pragma once
#include "chunk_core.h"
#include "chrono.h"
#include <ogc/gu.h>

void block_selection_init(void);
void block_selection_update(Mtx view);
void block_selection_draw(us_t now);
void block_selection_handle_location(Mtx view, world_location_t location);