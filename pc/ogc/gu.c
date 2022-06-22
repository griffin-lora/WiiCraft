#include <ogc/gu.h>

void c_guMtxConcat(f32 (*a)[4], f32 (*b)[4], f32 (*ab)[4]) {}
void guLookAt(f32 (*mt)[4], guVector *camPos, guVector *camUp, guVector *target) {}
void guPerspective(f32 (*mt)[4], f32 fovy, f32 aspect, f32 n, f32 f) {}
void guOrtho(f32 (*mt)[4], f32 t, f32 b, f32 l, f32 r, f32 n, f32 f) {}
void c_guMtxIdentity(f32 (*mt)[4]) {}
void c_guMtxTransApply(f32 (*src)[4], f32 (*dst)[4], f32 xT, f32 yT, f32 zT) {}