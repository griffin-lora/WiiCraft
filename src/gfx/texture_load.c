#include "texture_load.h"
#ifndef PC_PORT
#include "../build/textures_tpl.h"
#include "../build/textures.h"
#endif
#include <ogc/tpl.h>

game_textures_t load_game_textures() {
    game_textures_t textures;

    #ifndef PC_PORT
    TPLFile file;
    TPL_OpenTPLFromMemory(&file, (void*)textures_tpl, textures_tpl_size);

    TPL_GetTexture(&file, chunk_tex, &textures.chunk);
    TPL_GetTexture(&file, icons_tex, &textures.icons);
    TPL_GetTexture(&file, skybox_tex, &textures.skybox);
    TPL_GetTexture(&file, font_tex, &textures.font);
    #endif

    return textures;
}