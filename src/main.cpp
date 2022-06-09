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
#include "input.hpp"

constexpr f32 cam_move_speed = 0.3f;
constexpr math::vector2f cam_rotation_speed = { 2.0f, 1.0f };

int main(int argc, char** argv) {

	gfx::console_state con;
	gfx::init(con);

	if (!fatInitDefault()) {
		printf("fatInitDefault() failed!\n");
		dbg::freeze();
	}

	gfx::texture texture;
	gfx::safe_load_from_file(texture, "data/textures/chunk.tpl");

	input::init(con.rmode->viWidth, con.rmode->viHeight);

	gfx::draw_state draw;
	gfx::init(draw, {0x0, 0x0, 0x0, 0xFF});

	GX_SetCullMode(GX_CULL_BACK);

	gfx::set_filtering_mode(texture, GX_NEAR, GX_NEAR);

	math::matrix view; // view and perspective matrices
	math::matrix44 perspective;

	game::camera cam = {
		.position = {0.0f, 3.0f, -10.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.rotation = {
			{0.0f, 0.0f, 1.0f},
			{0.0f, 1.0f, 0.0f},
			{1.0f, 0.0f, 0.0f}
		},
		.fov = 45.0f,
		.aspect = (f32)((f32)draw.rmode->viWidth / (f32)draw.rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	cam.rotation.normalize();
	game::camera_update_params cam_upd;

	game::update_view(cam, view);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	game::update_perspective(cam, perspective);
	gfx::set_projection_matrix(perspective, GX_PERSPECTIVE);

	bool first_frame = true;

	std::vector<game::chunk> chunks;

	for (s32 i = -1; i <= 1; i++) {
		for (s32 j = -1; j <= 1; j++) {
			auto size = game::get_face_vertex_count<game::block::face::TOP>(game::block::type::GRASS) * 1024;
			chunks.push_back({ .ms = { { size } }, .position = { i, 0, j } });
			auto& chunk = chunks.back();
			auto it = chunk.ms.vertices.begin();
			for (u8 k = 0; k < 32; k++) {
				for (u8 l = 0; l < 32; l++) {
					game::add_face_vertices_at_mut_it<game::block::face::TOP>({k, 0, l}, it, game::block::type::GRASS);
				}
			}
			if (it != chunk.ms.vertices.end()) {
				dbg::error([it, &chunk]() {
					std::printf("0x%p, 0x%p\n", &(*it), &(*chunk.ms.vertices.end()));
				});
			}
		}
	}
	
	for (auto& chunk : chunks) {
		game::init(chunk, view);
	}

	math::vector2f video_size = {(f32)draw.rmode->viWidth, (f32)draw.rmode->viHeight};
	math::vector2f video_size_reciprocal = {1.f / video_size.x, 1.f / video_size.y};

	input::state inp;

	for (;;) {

		WPAD_ScanPads();
		u32 buttons_held = input::get_buttons_held(0);
		if (buttons_held & WPAD_BUTTON_HOME) { std::exit(0); }

		math::vector2f pointer_input_vector = input::get_pointer_input_vector(inp, buttons_held);
		if (pointer_input_vector.is_non_zero()) {
			pointer_input_vector *= video_size_reciprocal * cam_rotation_speed * -1;
			
			game::rotate_camera(cam, pointer_input_vector.x, pointer_input_vector.y);
			
			cam_upd.update_view = true;
		}

		math::vector3f pad_input_vector = input::get_dpad_input_vector(buttons_held);

		if (pad_input_vector.is_non_zero()) {
			game::move_camera_from_input_vector(cam, pad_input_vector, cam_move_speed);
			
			cam_upd.update_view = true;
		}


		if (cam_upd.update_view) {
			game::update_view(cam, view);
		}
		if (cam_upd.update_perspective) {
			game::update_perspective(cam, perspective);
		}

		gfx::set_channel_count(1);
		gfx::set_texture_coordinate_generation_count(1);

		// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
		gfx::init_texture_coordinate_generation(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		gfx::set_tev_operation(GX_TEVSTAGE0,GX_REPLACE);
		gfx::set_tev_order(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

		gfx::load(texture);

		// guMtxRotAxisDeg(model, &cube_axis, rquad);
		if (cam_upd.update_view) {
			for (auto& chunk : chunks) {
				game::update_model_view(chunk, view);
				game::draw_chunk(chunk);
			}
		} else {
			for (auto& chunk : chunks) {
				game::draw_chunk(chunk);
			}
		}

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

		// rquad -= 0.15f;				// Decrease The Rotation Variable For The Quad     ( NEW )
	}
}
