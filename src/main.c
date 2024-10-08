#include "asset.h"
#include "game/region.h"
#include "game/region_rendering.h"
#include "game/voxel.h"
#include "gfx.h"
#include "game_math.h"
#include "input.h"
#include "chrono.h"
#include "game/voxel_raycast.h"
#include "game/camera.h"
#include "game/logic.h"
#include "game/voxel_selection.h"
#include "game/cursor.h"
#include "game/skybox.h"
#include "game/character.h"
#include "game/rendering.h"
#include "game/water_overlay.h"
#include "game/debug_ui.h"
#include "log.h"
#include "game/region_management.h"
#include <cglm/struct/mat4.h>
#include <ogc/gu.h>
#include <stdlib.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <math.h>

int main(int, char**) {
	if (!log_init()) {
		return 1;
	}

	lprintf("Log started\n");

	if (!gfx_init()) {
		return 1;
	}

	asset_init();

	input_init(render_mode->viWidth, render_mode->viHeight);

	mat4s projection_2d;
	guOrtho(projection_2d.raw, 0.0f, 479.0f, 0.0f, 639.0f, 0.0f, 300.0f);
	
	mat4s projection_3d;
	mat4s view;
	
	aspect = (f32) render_mode->viWidth / (f32) render_mode->viHeight;
	glm_vec3_normalize(cam_forward.raw);

	camera_update_visuals(0, &projection_3d, &view);

	#ifdef PC_PORT
	u16 num_frames = 0;
	#endif

	skybox_init(&view, cam_position);
	
	water_overlay_init();
	debug_ui_init();
	
	cursor_init();

	voxel_selection_init();

	init_ui_rendering();
	init_region_rendering();

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);

	vec3w_t last_wpad_accel = { 512, 512, 512 };
	vec3w_t last_nunchuk_accel = { 512, 512, 512 };

	s64 program_start = get_current_us();
	us_t start = 0;
	
	s32vec3s last_region_pos = get_region_position_from_voxel_world_position(get_voxel_world_position(cam_position));

	init_region_management();

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
			lprintf("BGT: %d\nMGT: %d\nMGL: %d\nLog ended\n", total_procedural_gen_time, total_visual_gen_time, last_visual_gen_time);
			log_term();
			exit(0);
		}
		u32 buttons_held = WPAD_ButtonsHeld(chan);

		camera_update(frame_delta, buttons_held);

		s32vec3s region_pos = get_region_position_from_voxel_world_position(get_voxel_world_position(cam_position));
		manage_regions(last_region_pos, region_pos);
		last_region_pos = region_pos;

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
		character_handle_input((vec3w_t) { 0, 0, 0 }, (vec3w_t) { 0, 0, 0 }, now, frame_delta, (vec3w_t) { 512, 512, 512 }, (vec2s) {{ 96.0f, 96.0f }}, 0, (vec3w_t) { 512, 512, 512 });
		#endif

		vec3s raycast_dir = cam_forward;
		voxel_raycast_wrap_t raycast = get_voxel_raycast(cam_position, glms_vec3_scale(raycast_dir, 10.0f), cam_position, glms_vec3_add(cam_position, glms_vec3_scale(raycast_dir, 10.0f)), (vec3s){ .x = 0, .y = 0, .z = 0 }, voxel_box_type_selection);

		if (raycast.success) {
			voxel_selection_update(&view, raycast.val.voxel_world_pos);
			update_world(&raycast.val, buttons_down);
		}
		
		character_apply_physics(frame_delta);
		character_apply_velocity(frame_delta);
		
		camera_update_visuals(now, &projection_3d, &view);

		skybox_update(&view, cam_position);
		voxel_selection_update_view(&view);
		debug_ui_update(buttons_down);

		GX_LoadProjectionMtx(projection_3d.raw, GX_PERSPECTIVE);
		skybox_draw();

		GX_SetCurrentMtx(REGION_MATRIX_INDEX);

		draw_regions(&view);
		
		if (raycast.success) {
			voxel_selection_draw(now);
		}
		
		GX_LoadProjectionMtx(projection_2d.raw, GX_ORTHOGRAPHIC);
		enter_ui_rendering();
		water_overlay_draw();
		cursor_draw();

		enter_text_rendering();
		debug_ui_draw(total_procedural_gen_time, total_visual_gen_time, last_visual_gen_time, (u32) ceilf(fps));

		gfx_update_video();
		
		#ifdef PC_PORT
		if (++num_frames == 1200) {
			printf("BGT: %ld\nMGT: %ld\n", total_procedural_gen_time, total_visual_gen_time);
			exit(0);
		}
		#endif
	}
}
