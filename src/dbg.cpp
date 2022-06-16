#include "dbg.hpp"
#include <wiiuse/wpad.h>
#include <gccore.h>
#include <cstdlib>

using namespace dbg;

void dbg::freeze() {
    for (;;) {
		// WPAD_ScanPads();

		// if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
		// 	std::exit(1);
		// }

		// VIDEO_WaitVSync();
	}
}