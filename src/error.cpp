#include "error.hpp"
#include <wiiuse/wpad.h>
#include <cstdlib>

void freeze() {
    for (;;) {
        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) {
            std::exit(0);
        }
    }
}