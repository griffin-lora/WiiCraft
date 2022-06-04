#include "gfx.hpp"
#include <cstdio>

using namespace gfx;

void gfx::init(console_state& s) {
    // Initialise the video system
	VIDEO_Init();

	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	s.rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	s.xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(s.rmode));

	// Initialise the console, required for printf
	console_init(s.xfb,20,20,s.rmode->fbWidth,s.rmode->xfbHeight,s.rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(s.rmode);

	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(s.xfb);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(s.rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();


	// The console understands VT terminal escape codes
	// This positions the cursor on row 2, column 0
	// we can use variables for this with format codes too
	// e.g. printf ("\x1b[%d;%dH", row, column );
	std::printf("\x1b[2;0H");
}