
#include <cstdio>
#include <cstdlib>
#include <wiiuse/wpad.h>
#include "gfx.hpp"

int main(int argc, char** argv) {
	// This function initialises the attached controllers
	WPAD_Init();

	enable_gfx_mode(gfx_mode::console);

	std::printf("Hello World!");

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
		wait_for_vsync();
	}

	return 0;
}
