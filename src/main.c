#include "asset.h"
#include "gfx.h"
#include "game_math.h"
#include "input.h"
#include "chrono.h"
#include "game/block_raycast.h"
#include "game/camera.h"
#include "game/block_world_rendering.h"
#include "game/logic.h"
#include "game/block_selection.h"
#include "game/cursor.h"
#include "game/skybox.h"
#include "game/character.h"
#include "game/rendering.h"
#include "game/water_overlay.h"
#include "game/debug_ui.h"
#include "log.h"
#include "pool.h"
#include "game/block_world_management.h"
#include <cglm/struct/mat4.h>
#include <stdlib.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <math.h>

static s32vec3s corner_pos_offset = { .x = -3, .y = -2, .z = -3 };

int main(int, char**) {
	if (!log_init()) {
		return 1;
	}

	lprintf("Log started\n");

	if (!gfx_init()) {
		return 1;
	}

	asset_init();

	pool_init();

	input_init(render_mode->viWidth, render_mode->viHeight);

	mat4s projection_2d = glms_ortho(0.0f, 639.0f, 479.0f, 0.0f, 0.0f, 300.0f);
	projection_2d = glms_mat4_transpose(projection_2d);
	
	mat4s view;
	mat4s projection_3d;
	
	aspect = (f32) render_mode->viWidth / (f32) render_mode->viHeight;
	glm_vec3_normalize(cam_forward.raw);

	camera_update_visuals(0, &view, &projection_3d);

	#ifdef PC_PORT
	u16 num_frames = 0;
	#endif

	skybox_init(&view, cam_position);
	
	water_overlay_init();
	debug_ui_init();
	
	cursor_init();

	block_selection_init();

	init_ui_rendering();
	init_block_world_rendering();

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);

	vec3w_t last_wpad_accel = { 512, 512, 512 };
	vec3w_t last_nunchuk_accel = { 512, 512, 512 };

	s64 program_start = get_current_us();
	us_t start = 0;
	
	vec3s div_pos = glms_vec3_scale(cam_position, 1.0f/(f32)NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
	s32vec3s last_corner_pos = { .x = (s32)floorf(div_pos.x), .y = (s32)floorf(div_pos.y), .z = (s32)floorf(div_pos.z) };
	glm_ivec3_add(last_corner_pos.raw, corner_pos_offset.raw, last_corner_pos.raw);

	init_block_world(last_corner_pos);

	for (;;) {
        us_t now = (us_t) (get_current_us() - program_start);

		us_t delta_time = now - start;
		f32 frame_delta = (f32) delta_time / 1000000.0f;
		f32 fps = 1.0f / frame_delta;

		start = now;

		s32 chan = 0;

		WPAD_ScanPads();
		u32 buttons_down = WPAD_ButtonsDown(chan);
		if (buttons_down & WPAD_BUTTON_HOME) {
			lprintf("BGT: %d\nMGT: %d\nMGL: %d\nLog ended\n", total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time);
			log_term();
			exit(0);
		}
		u32 buttons_held = WPAD_ButtonsHeld(chan);

		camera_update(frame_delta, buttons_held);

		vec3s div_pos = glms_vec3_scale(cam_position, 1.0f/(f32)NUM_ROW_BLOCKS_PER_BLOCK_CHUNK);
		s32vec3s corner_pos = { .x = (s32)floorf(div_pos.x), .y = (s32)floorf(div_pos.y), .z = (s32)floorf(div_pos.z) };
		glm_ivec3_add(corner_pos.raw, corner_pos_offset.raw, corner_pos.raw);
		if (last_corner_pos.x != corner_pos.x || last_corner_pos.y != corner_pos.y || last_corner_pos.z != corner_pos.z) {
			manage_block_world(last_corner_pos, corner_pos);
		}
		handle_world_flag_processing(corner_pos);
		last_corner_pos = corner_pos;

		cursor_update(render_mode->viWidth, render_mode->viHeight);

		#ifndef PC_PORT
    	vec3w_t wpad_accel;
		WPAD_Accel(chan, &wpad_accel);
		expansion_t exp;
		WPAD_Expansion(chan, &exp);
		if (exp.type == WPAD_EXP_NUNCHUK) {
			const nunchuk_t* nunchuk = &exp.nunchuk;
			vec2s nunchuk_vector = get_nunchuk_vector(nunchuk);
			u8 nunchuk_buttons_down = nunchuk->btns;

			vec3w_t nunchuk_accel = { nunchuk->accel.x, nunchuk->accel.y, nunchuk->accel.z };

			character_handle_input(last_wpad_accel, last_nunchuk_accel, now, frame_delta, wpad_accel, nunchuk_vector, nunchuk_buttons_down, nunchuk_accel);

			last_nunchuk_accel = nunchuk_accel;
			// character.velocity.y = get_plus_minus_input_scalar(buttons_held) * 15.0f;
		}
		last_wpad_accel = wpad_accel;
		#else
		character_handle_input(cam.look, (vec3w_t){ 0, 0, 0 }, (vec3w_t){ 0, 0, 0 }, now, frame_delta, (vec3w_t){ 512, 512, 512 }, (vec3w_t){ 96.0f, 96.0f }, 0, (vec3w_t){ 512, 512, 512 });
		#endif

		// auto raycast_dir = get_raycast_direction_from_pointer_position(rmode->viWidth, rmode->viHeight, cam, pointer_pos);
		vec3s raycast_dir = cam_forward;
		block_raycast_wrap_t raycast = get_block_raycast(corner_pos, cam_position, glms_vec3_scale(raycast_dir, 10.0f), cam_position, glms_vec3_add(cam_position, glms_vec3_scale(raycast_dir, 10.0f)), (vec3s){ .x = 0, .y = 0, .z = 0 }, block_box_type_selection);

		if (raycast.success) {
			block_selection_handle_location(&view, raycast.val.location);
			update_world_from_location_and_input(corner_pos, buttons_down, raycast.val.location, glms_vec3_add(raycast.val.world_block_position, raycast.val.box_raycast.normal));
		}
		character_apply_physics(corner_pos, frame_delta);
		character_apply_velocity(frame_delta);
		
		camera_update_visuals(now, &view, &projection_3d);

		skybox_update(&view, cam_position);
		block_selection_update(&view);
		debug_ui_update(buttons_down);

		GX_LoadProjectionMtx(projection_3d.raw, GX_PERSPECTIVE);
		skybox_draw();

		GX_SetCurrentMtx(BLOCK_WORLD_MATRIX_INDEX);

		draw_block_display_lists(&view);
		
		if (raycast.success) {
			block_selection_draw(now);
		}
		
		GX_LoadProjectionMtx(projection_2d.raw, GX_ORTHOGRAPHIC);
		enter_ui_rendering();
		water_overlay_draw();
		cursor_draw();

		enter_text_rendering();
		debug_ui_draw(total_block_gen_time, total_mesh_gen_time, last_mesh_gen_time, (u32) ceilf(fps));

		gfx_update_video();
		
		#ifdef PC_PORT
		if (++num_frames == 1200) {
			std::printf("BGT: %ld\nMGT: %ld\n", total_block_gen_time, total_mesh_gen_time);
			std::exit(0);
		}
		#endif
	}
}
