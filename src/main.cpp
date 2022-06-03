
#include <cstdio>
#include <cstdlib>
#include <wiiuse/wpad.h>
#include "gfx.hpp"
#include "error.hpp"
#include <fstream>
#include <fat.h>

int main(int argc, char** argv) {

	// This function initialises the attached controllers
	WPAD_Init();

	gfx::enable_mode(gfx::mode::console);

	if (!fatInitDefault()) {
		std::puts("fatInitDefault failed");
		freeze();
	}

	gfx::texture chunk_texture;
	if (!chunk_texture.load_from_file("data/textures/chunk.tpl")) {
		std::printf("Failed to load chunk texture\n");
		freeze();
	}
	chunk_texture.use();

	std::puts("Hello World!");

	while (true) {

		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if (pressed & WPAD_BUTTON_HOME) {
			std::exit(0);
		}

		// Wait for the next frame
		gfx::wait_for_vsync();
	}

	return 0;
}
