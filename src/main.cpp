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
#include "game/block_logic.hpp"
#include "game/util.hpp"
#include "game/chunk_mesh_generation.hpp"
#include "game/chunk_rendering.hpp"
#include "game/stored_chunk.hpp"
#include "game/block_selection.hpp"
#include "game/chunk_management.hpp"

constexpr f32 cam_move_speed = 0.15f;
constexpr f32 cam_rotation_speed = 0.15f;

constexpr s32 chunk_generation_radius = 2;

constexpr s32 chunk_erasure_radius = 3;

int main(int argc, char** argv) {

	gfx::console_state con;
	gfx::init(con);

	if (!fatInitDefault()) {
		printf("fatInitDefault() failed!\n");
		dbg::freeze();
	}

	gfx::texture chunk_tex;
	gfx::safe_load_from_file(chunk_tex, "data/textures/chunk.tpl");

	input::init(con.rmode->viWidth, con.rmode->viHeight);

	gfx::draw_state draw;
	gfx::init(draw, {0xFF, 0xFF, 0xFF, 0xFF});

	GX_SetCullMode(GX_CULL_BACK);

	gfx::set_filtering_mode(chunk_tex, GX_NEAR, GX_NEAR);

	math::matrix view;
	math::matrix44 perspective;

	game::camera cam = {
		.position = {0.0f, 3.0f, -10.0f},
		.up = {0.0f, 1.0f, 0.0f},
		.look = {0.0f, 0.0f, 1.0f},
		.fov = 90.0f,
		.aspect = (f32)((f32)draw.rmode->viWidth / (f32)draw.rmode->viHeight),
		.near_clipping_plane_distance = 0.1f,
		.far_clipping_plane_distance = 300.0f
	};
	std::optional<math::vector3s32> last_cam_chunk_pos;
	math::normalize(cam.look);

	game::update_look(cam);
	game::update_view(cam, view);

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	game::update_perspective(cam, perspective);
	gfx::set_projection_matrix(perspective, GX_PERSPECTIVE);

	bool first_frame = true;

	game::chunk::map chunks;
	// This are variables whose lifetime is bound to the update_mesh function normally. However, since it takes up quite a bit of memory, it is stored here.
	ext::data_array<game::block::face_cache> face_caches(game::chunk::BLOCKS_COUNT);
	game::stored_chunk::map stored_chunks;

	input::state inp;

	std::vector<math::vector3s32> inserted_chunk_positions;

	game::block_selection bl_sel;

	for (;;) {
		// Check for selected block
		auto raycast = game::get_raycast(cam, chunks);
		game::handle_raycast(view, bl_sel, raycast);

		input::handle(cam_move_speed, cam_rotation_speed, cam, raycast);

		game::manage_chunks_around_camera(chunk_erasure_radius, chunk_generation_radius, view, cam, last_cam_chunk_pos, chunks, stored_chunks, inserted_chunk_positions);

		game::update_needed(view, perspective, cam);

		game::update_chunks(chunks, face_caches);

		game::draw_chunks(chunk_tex, view, cam, chunks);

		game::draw_block_selection(view, cam, bl_sel, raycast);

		game::reset_update_params(cam);

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
	}
}
