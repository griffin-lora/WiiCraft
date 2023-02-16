#pragma once
#include <ogc/gu.h>

#define BLOCK_WORLD_MATRIX_INDEX GX_PNMTX5
#define BLOCK_WORLD_VERTEX_FORMAT_INDEX GX_VTXFMT5

void init_block_world_rendering(void);
void draw_block_display_lists(Mtx view);