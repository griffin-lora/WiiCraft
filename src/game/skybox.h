#pragma once
#include <gctypes.h>
#include <ogc/gu.h>

void skybox_init(Mtx view, f32 x, f32 y, f32 z);
void skybox_update(Mtx view, f32 x, f32 y, f32 z);
void skybox_draw(void);