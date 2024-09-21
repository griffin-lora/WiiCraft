#pragma once
#include "game_math.h"
#include <gctypes.h>
#include <ogc/gu.h>

void skybox_init(const mat4s* view, vec3s cam_pos);
void skybox_update(const mat4s* view, vec3s cam_pos);
void skybox_draw(void);