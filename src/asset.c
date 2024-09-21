#include "asset.h"
#ifndef PC_PORT
#include "../build/textures_tpl.h"
#include "../build/textures.h"
#endif
#include <ogc/gx.h>
#include <ogc/tpl.h>

alignas(32) static struct {
	GXTexObj chunk;
	GXTexObj icons;
	GXTexObj skybox;
	GXTexObj font;
} textures;

void asset_init(void) {
    #ifndef PC_PORT
    TPLFile file;
    TPL_OpenTPLFromMemory(&file, (void*)textures_tpl, textures_tpl_size);

    TPL_GetTexture(&file, chunk_tex, &textures.chunk);
    TPL_GetTexture(&file, icons_tex, &textures.icons);
    TPL_GetTexture(&file, skybox_tex, &textures.skybox);
    TPL_GetTexture(&file, font_tex, &textures.font);
    #endif

	GX_InitTexObjFilterMode(&textures.chunk, GX_NEAR, GX_NEAR);
	GX_InitTexObjFilterMode(&textures.icons, GX_NEAR, GX_NEAR);
	GX_InitTexObjFilterMode(&textures.font, GX_NEAR, GX_NEAR);

	GX_LoadTexObj(&textures.chunk, GX_TEXMAP0);
	GX_LoadTexObj(&textures.icons, GX_TEXMAP1);
	GX_LoadTexObj(&textures.skybox, GX_TEXMAP2);
	GX_LoadTexObj(&textures.font, GX_TEXMAP3);
}