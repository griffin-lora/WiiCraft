#include "dbg.hpp"
#include <cstdlib>
#include <gccore.h>
#include <wiiuse/wpad.h>

using namespace dbg;

void dbg::freeze() {
    WPAD_Init();

    for (;;) {
		WPAD_ScanPads();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
			std::exit(1);
		}

		VIDEO_WaitVSync();
	}
}