#pragma once
#include <cglm/struct/mat4.h>

#define REGION_MATRIX_INDEX GX_PNMTX5
#define REGION_VERTEX_FORMAT_INDEX GX_VTXFMT5

void init_region_rendering(void);
void draw_regions(const mat4s* view);