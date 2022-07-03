#include "rendering.hpp"
#include <gccore.h>

using namespace game;

void game::init_standard_rendering() {
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);
}

void game::init_foliage_rendering() {
	GX_SetAlphaCompare(GX_GEQUAL, 0x20, GX_AOP_AND, GX_ALWAYS, 0); // I don't know why this works but it does, see https://github.com/devwizard64/metapro/blob/bda8d24556ba160a339ade631469dffe2a1cf752/src/gdp/set_rm.gekko.c

	GX_SetZCompLoc(GX_FALSE);
	GX_SetCullMode(GX_CULL_NONE);
}

void game::init_water_rendering() {
	GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetZCompLoc(GX_TRUE);
	GX_SetCullMode(GX_CULL_BACK);
}