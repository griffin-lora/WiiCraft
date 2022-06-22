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
#include "input.hpp"
#include <map>
#include <numeric>
#include "game/camera.hpp"
#include "game/chunk_core.hpp"
#include "game/chunk_management.hpp"
#include "game/chunk_rendering.hpp"
#include "game/stored_chunk.hpp"
#include "game/logic.hpp"
#include "game/block_selection.hpp"
#include "game/cursor.hpp"
#include "game/skybox.hpp"
#include "game/character.hpp"
#include "common.hpp"

constexpr f32 cam_move_speed = 9.0f;
constexpr f32 cam_rotation_speed = 0.15f;
constexpr f32 gravity = 0.6f;

constexpr s32 chunk_generation_radius = 3;

constexpr s32 chunk_erasure_radius = 4;

int main(int argc, char** argv) {

	gfx::console_state con;

	if (!fatInitDefault()) {
		printf("fatInitDefault() failed!\n");
		dbg::freeze();
	}

	gfx::texture chunk_tex;
	gfx::safe_load_from_file(chunk_tex, "data/textures/chunk.tpl");

	gfx::texture icons_tex;
	gfx::safe_load_from_file(icons_tex, "data/textures/icons.tpl");

	gfx::texture skybox_tex;
	gfx::safe_load_from_file(skybox_tex, "data/textures/skybox.tpl");

	input::init(con.rmode->viWidth, con.rmode->viHeight);

	gfx::draw_state draw{ {0xFF, 0xFF, 0xFF, 0xFF} };

	input::set_resolution(draw.rmode->viWidth, draw.rmode->viHeight);

	gfx::set_filtering_mode(chunk_tex, GX_NEAR, GX_NEAR);
	gfx::set_filtering_mode(icons_tex, GX_NEAR, GX_NEAR);

	math::matrix44 perspective_2d;
	guOrtho(perspective_2d, 0, 479, 0, 639, 0, 300);
	
	math::matrix view;
	math::matrix44 perspective_3d;

	game::character character = {
		.position = { 0.0f, 0.0f, 0.0f },
		.velocity = { 0.0f, 0.0f, 0.0f }
	};

	game::camera cam = {
		.position = {0.0f, 30.0f, -10.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.look = {0.0f, 0.0f, 1.0f},
		.fov = 90.0f,
		.aspect = (f32)((f32)draw.rmode->viWidth / (f32)draw.rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	character.update_camera(cam);
	std::optional<math::vector3s32> last_cam_chunk_pos;
	math::normalize(cam.look);

	game::update_look(cam);
	game::update_view(cam, view);

	game::update_perspective(cam, perspective_3d);

	bool first_frame = true;

	game::chunk::map chunks;
	// This is a variable whose lifetime is bound to the update_mesh function normally. However, since it takes up quite a bit of memory, it is stored here.
	ext::data_array<game::chunk::vertex> building_vertices(game::chunk::MAX_VERTEX_COUNT + 0x30);
	game::stored_chunk::map stored_chunks;

	input::state inp;

	std::vector<math::vector3s32> inserted_chunk_positions;

	game::skybox skybox{view, cam};
	
	game::cursor cursor;

	game::block_selection bl_sel;

    GX_SetCurrentMtx(GX_PNMTX3);

	for (;;) {
		auto raycast = game::get_raycast(cam.position, cam.look, 1024, chunks);
		bl_sel.handle_raycast(view, raycast);

		game::update_from_input(cam_move_speed, cam_rotation_speed, draw.rmode->viWidth, draw.rmode->viHeight, character, cam, chunks, cursor, raycast);
		character.apply_physics(gravity, chunks);
		character.update_camera(cam);

		game::manage_chunks_around_camera(chunk_erasure_radius, chunk_generation_radius, view, cam, last_cam_chunk_pos, chunks, stored_chunks, inserted_chunk_positions);


		game::update_needed(view, perspective_3d, cam);

		game::update_chunks(chunks, building_vertices);

		GX_LoadProjectionMtx(perspective_3d, GX_PERSPECTIVE);
		skybox.update_if_needed(view, cam);
		skybox.draw(skybox_tex);
		game::draw_chunks(chunk_tex, view, cam, chunks);
		bl_sel.update_if_needed(view, cam);
		bl_sel.draw(raycast);
		
		GX_LoadProjectionMtx(perspective_2d, GX_ORTHOGRAPHIC);
		cursor.draw(icons_tex);

		game::reset_update_params(cam);

		gfx::set_z_buffer_mode(true, GX_LEQUAL, true);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);

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
	}
}
