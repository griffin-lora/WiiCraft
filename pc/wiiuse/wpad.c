#include <gccore.h>
#include <wiiuse/wpad.h>

s32 WPAD_Init(void) { return 0; }
s32 WPAD_SetVRes(s32 chan, u32 xres, u32 yres) { return 0; }
s32 WPAD_SetDataFormat(s32 chan, s32 fmt) { return 0; }

s32 WPAD_ScanPads(void) { return 0; }
u32 WPAD_ButtonsDown(int chan) { return 0; }
u32 WPAD_ButtonsHeld(int chan) { return 0; }

void WPAD_IR(int chan, struct ir_t *ir) {}
void WPAD_Expansion(int chan, struct expansion_t *exp) {}