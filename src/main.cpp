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
#include "game.hpp"

int main(int argc, char** argv) {

	gfx::console_state con;
	gfx::init(con);

	if (!fatInitDefault()) {
		printf("fatInitDefault() failed!\n");
		dbg::freeze();
	}

	gfx::texture texture;

	gfx::safe_load_from_file(texture, "data/textures/chunk.tpl");

	WPAD_Init();

	gfx::draw_state draw;
	gfx::init(draw, {0x0, 0x0, 0x0, 0xFF});

	gfx::set_filtering_mode(texture, GX_NEAR, GX_NEAR);

	math::matrix view; // view and perspective matrices
	math::matrix44 perspective;

	// setup our camera at the origin
	// looking down the -z axis with y up

	game::camera cam = {
		.position = {0.0f, 0.0f, -7.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.look = {0.0f, 0.0f, -1.0f},
		.fov = 45.0f,
		.aspect = (f32)((f32)draw.rmode->viWidth / (f32)draw.rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	game::camera_update_params cam_upd;

	game::update_view_from_camera(cam, view);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	game::update_perspective_from_camera(cam, perspective);
	gfx::set_projection_matrix(perspective, GX_PERSPECTIVE);

	math::vector3 cube_axis = {1,1,1};

	f32 rquad = 0.0f;
	bool first_frame = true;

	for (;;) {

		WPAD_ScanPads();
		u32 pad_buttons_down = WPAD_ButtonsHeld(0);
		if (pad_buttons_down & WPAD_BUTTON_HOME) { std::exit(0); }
		if (pad_buttons_down & WPAD_BUTTON_UP) { cam.position.z -= 0.25f; cam_upd.update_view = true; }
		if (pad_buttons_down & WPAD_BUTTON_DOWN) { cam.position.z += 0.25f; cam_upd.update_view = true; }

		if (cam_upd.update_view) {
			game::update_view_from_camera(cam, view);
		}
		if (cam_upd.update_perspective) {
			game::update_perspective_from_camera(cam, perspective);
		}

		gfx::set_channel_count(1);
		gfx::set_texture_coordinate_generation_count(1);

		// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
		gfx::init_texture_coordinate_generation(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		gfx::set_tev_operation(GX_TEVSTAGE0,GX_REPLACE);
		gfx::set_tev_order(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

		gfx::load(texture);

		math::matrix model;
		math::matrix model_view;
		guMtxIdentity(model);
		guMtxRotAxisDeg(model, &cube_axis, rquad);
		guMtxTransApply(model, model, 0.0f, 0.0f, 0.f);
		guMtxConcat(view,model,model_view);
		// load the modelview matrix into matrix memory
		gfx::set_position_matrix_into_index(model_view, GX_PNMTX3);
		
		gfx::set_position_matrix_from_index(GX_PNMTX3);

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

		gfx::set_z_buffer_mode(true, GX_LEQUAL, true);
		gfx::set_color_buffer_update(true);

		gfx::copy_framebuffer(draw.frame_buffers[draw.fb_index], true);

		gfx::draw_done();

		VIDEO_SetNextFramebuffer(draw.frame_buffers[draw.fb_index]);
		if (first_frame) {
			first_frame = false;
			VIDEO_SetBlack(FALSE);
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		draw.fb_index ^= 1;

		// Game logic

		rquad -= 0.15f;				// Decrease The Rotation Variable For The Quad     ( NEW )
	}
}
