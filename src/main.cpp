/*---------------------------------------------------------------------------------

	nehe lesson 6 port to GX by shagkur

---------------------------------------------------------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <cmath>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <array>
#include "gfx.hpp"
#include "dbg.hpp"


constexpr gfx::color3 VERT_COLOR = {0xFF, 0xFF, 0xFF};

int main(int argc, char** argv) {

	WPAD_Init();

	if (!fatInitDefault()) {
		dbg::error([]() {
			printf("fatInitDefault() failed!\n");
		});
	}

	gfx::draw_state draw;
	gfx::init(draw, {0x0, 0x0, 0x0, 0xFF});

	GXTexObj texture;

	TPLFile crateTPL;
	TPL_OpenTPLFromFile(&crateTPL, "data/textures/chunk.tpl");
	TPL_GetTexture(&crateTPL,0,&texture);

	GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);

	Mtx view; // view and perspective matrices

	// setup our camera at the origin
	// looking down the -z axis with y up

	guVector cam = {0.0F, 0.0F, 0.0F};
	guVector up = {0.0F, 1.0F, 0.0F};
	guVector look = {0.0F, 0.0F, -1.0F};

	guLookAt(view, &cam, &up, &look);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	f32 w = draw.rmode->viWidth;
	f32 h = draw.rmode->viHeight;
	Mtx44 perspective;
	guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	guVector cubeAxis = {1,1,1};

	f32 z_offset = -7.0f;
	f32 rquad = 0.0f;
	bool first_frame = true;

	for (;;) {

		WPAD_ScanPads();
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) { std::exit(0); }
		else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_UP) { z_offset -= 0.25f; }
		else if (WPAD_ButtonsHeld(0) & WPAD_BUTTON_DOWN) { z_offset += 0.25f; }

		// set number of rasterized color channels
		GX_SetNumChans(1);

		//set number of textures to generate
		GX_SetNumTexGens(1);

		// setup texture coordinate generation
		// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

		GX_LoadTexObj(&texture, GX_TEXMAP0);

		Mtx model;
		Mtx modelview;
		guMtxIdentity(model);
		guMtxRotAxisDeg(model, &cubeAxis, rquad);
		guMtxTransApply(model, model, 0.0f,0.0f,z_offset);
		guMtxConcat(view,model,modelview);
		// load the modelview matrix into matrix memory
		GX_LoadPosMtxImm(modelview, GX_PNMTX3);
		GX_SetCurrentMtx(GX_PNMTX3);

		GX_Begin(GX_QUADS, GX_VTXFMT0, 24);			// Draw a Cube

		GX_Position3f32(-1.0f, 1.0f, -1.0f);	// Top Left of the quad (top)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(-1.0f, 1.0f, 1.0f);	// Top Right of the quad (top)
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(-1.0f, -1.0f, 1.0f);	// Bottom Right of the quad (top)
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32(- 1.0f, -1.0f, -1.0f);		// Bottom Left of the quad (top)
		GX_TexCoord2f32(0.0f,1.0f);

		GX_Position3f32( 1.0f,1.0f, -1.0f);	// Top Left of the quad (bottom)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(1.0f,-1.0f, -1.0f);	// Top Right of the quad (bottom)
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(1.0f,-1.0f,1.0f);	// Bottom Right of the quad (bottom)
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32( 1.0f,1.0f,1.0f);	// Bottom Left of the quad (bottom)
		GX_TexCoord2f32(0.0f,1.0f);

		GX_Position3f32( -1.0f, -1.0f, 1.0f);		// Top Right Of The Quad (Front)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(1.0f, -1.0f, 1.0f);	// Top Left Of The Quad (Front)
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(1.0f,-1.0f, -1.0f);	// Bottom Left Of The Quad (Front)
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32( -1.0f,-1.0f, -1.0f);	// Bottom Right Of The Quad (Front)
		GX_TexCoord2f32(0.0f,1.0f);

		GX_Position3f32( -1.0f,1.0f,1.0f);	// Bottom Left Of The Quad (Back)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(-1.0f,1.0f,-1.0f);	// Bottom Right Of The Quad (Back)
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Back)
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32( 1.0f, 1.0f,1.0f);	// Top Left Of The Quad (Back)
		GX_TexCoord2f32(0.0f,1.0f);

		GX_Position3f32(1.0f, -1.0f, -1.0f);	// Top Right Of The Quad (Left)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32(1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
		GX_TexCoord2f32(1.0f,0.0f);
		GX_Position3f32(-1.0f,1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
		GX_TexCoord2f32(1.0f,1.0f);
		GX_Position3f32(-1.0f,-1.0f, -1.0f);	// Bottom Right Of The Quad (Left)
		GX_TexCoord2f32(0.0f,1.0f);

		GX_Position3f32( 1.0f, -1.0f,1.0f);	// Top Right Of The Quad (Right)
		GX_TexCoord2f32(0.0f,0.0f);
		GX_Position3f32( -1.0f, -1.0f, 1.0f);		// Top Left Of The Quad (Right)
		GX_TexCoord2f32(0.0625f,0.0f);
		GX_Position3f32( -1.0f,1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
		GX_TexCoord2f32(0.0625f,0.0625f);
		GX_Position3f32( 1.0f,1.0f,1.0f);	// Bottom Right Of The Quad (Right)
		GX_TexCoord2f32(0.0f,0.0625f);

		GX_End();									// Done Drawing The Quad

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(draw.frame_buffers[draw.fb_index],GX_TRUE);

		GX_DrawDone();

		VIDEO_SetNextFramebuffer(draw.frame_buffers[draw.fb_index]);
		if(first_frame) {
			first_frame = 0;
			VIDEO_SetBlack(FALSE);
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		draw.fb_index ^= 1;

		rquad -= 0.15f;				// Decrease The Rotation Variable For The Quad     ( NEW )
	}
}
