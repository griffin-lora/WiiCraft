#pragma once
#include <ogc/gx.h>

typedef struct {
    GXTexObj chunk;
    GXTexObj icons;
    GXTexObj skybox;
    GXTexObj font;
} game_textures_t;

game_textures_t load_game_textures();