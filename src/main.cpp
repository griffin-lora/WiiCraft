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
#include "math.hpp"

int main(int argc, char** argv) {

	WPAD_Init();

	if (!fatInitDefault()) {
		dbg::error([]() {
			printf("fatInitDefault() failed!\n");
		});
	}

	gfx::draw_state draw;
	gfx::init(draw, {0x0, 0x0, 0x0, 0xFF});

	gfx::texture texture;

	gfx::safe_load_from_file(texture, "data/textures/chunk.tpl");

	gfx::set_filtering_mode(texture, GX_NEAR, GX_NEAR);

	math::matrix view; // view and perspective matrices

	// setup our camera at the origin
	// looking down the -z axis with y up

	math::vector3 cam{0.0F, 0.0F, 0.0F};
	math::vector3 up{0.0F, 1.0F, 0.0F};
	math::vector3 look{0.0F, 0.0F, -1.0F};

	guLookAt(view, &cam, &up, &look);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	f32 w = draw.rmode->viWidth;
	f32 h = draw.rmode->viHeight;
	math::matrix44 perspective;
	guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	math::vector3 cubeAxis = {1,1,1};

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

		math::matrix model;
		math::matrix modelview;
		guMtxIdentity(model);
		guMtxRotAxisDeg(model, &cubeAxis, rquad);
		guMtxTransApply(model, model, 0.0f,0.0f,z_offset);
		guMtxConcat(view,model,modelview);
		// load the modelview matrix into matrix memory
		GX_LoadPosMtxImm(modelview, GX_PNMTX3);
		GX_SetCurrentMtx(GX_PNMTX3);

		gfx::draw_quads(24, []() {
			gfx::draw_vertex(
				-1.0f, 1.0f, -1.0f,	// Top Left of the quad (top)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				-1.0f, 1.0f, 1.0f,	// Top Right of the quad (top)
				1.0f,0.0f
			);
			gfx::draw_vertex(
				-1.0f, -1.0f, 1.0f,	// Bottom Right of the quad (top)
				1.0f,1.0f
			);
			gfx::draw_vertex(
				- 1.0f, -1.0f, -1.0f,		// Bottom Left of the quad (top)
				0.0f,1.0f
			);

			gfx::draw_vertex(
				1.0f,1.0f, -1.0f,	// Top Left of the quad (bottom)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f,-1.0f, -1.0f,	// Top Right of the quad (bottom)
				1.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f,-1.0f,1.0f,	// Bottom Right of the quad (bottom)
				1.0f,1.0f
			);
			gfx::draw_vertex(
				1.0f,1.0f,1.0f,	// Bottom Left of the quad (bottom)
				0.0f,1.0f
			);

			gfx::draw_vertex(
				-1.0f, -1.0f, 1.0f,		// Top Right Of The Quad (Front)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f, -1.0f, 1.0f,	// Top Left Of The Quad (Front)
				1.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f,-1.0f, -1.0f,	// Bottom Left Of The Quad (Front)
				1.0f,1.0f
			);
			gfx::draw_vertex(
				-1.0f,-1.0f, -1.0f,	// Bottom Right Of The Quad (Front)
				0.0f,1.0f
			);

			gfx::draw_vertex(
				-1.0f,1.0f,1.0f,	// Bottom Left Of The Quad (Back)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				-1.0f,1.0f,-1.0f,	// Bottom Right Of The Quad (Back)
				1.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f, 1.0f,-1.0f,	// Top Right Of The Quad (Back)
				1.0f,1.0f
			);
			gfx::draw_vertex(
				1.0f, 1.0f,1.0f,	// Top Left Of The Quad (Back)
				0.0f,1.0f
			);

			gfx::draw_vertex(
				1.0f, -1.0f, -1.0f,	// Top Right Of The Quad (Left)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				1.0f, 1.0f,-1.0f,	// Top Left Of The Quad (Left)
				1.0f,0.0f
			);
			gfx::draw_vertex(
				-1.0f,1.0f,-1.0f,	// Bottom Left Of The Quad (Left)
				1.0f,1.0f
			);
			gfx::draw_vertex(
				-1.0f,-1.0f, -1.0f,	// Bottom Right Of The Quad (Left)
				0.0f,1.0f
			);

			gfx::draw_vertex(
				1.0f, -1.0f,1.0f,	// Top Right Of The Quad (Right)
				0.0f,0.0f
			);
			gfx::draw_vertex(
				-1.0f, -1.0f, 1.0f,		// Top Left Of The Quad (Right)
				0.0625f,0.0f
			);
			gfx::draw_vertex(
				-1.0f,1.0f, 1.0f,	// Bottom Left Of The Quad (Right)
				0.0625f,0.0625f
			);
			gfx::draw_vertex(
				1.0f,1.0f,1.0f,	// Bottom Right Of The Quad (Right)
				0.0f,0.0625f
			);
		});

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
