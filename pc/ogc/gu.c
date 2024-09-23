#include <ogc/gu.h>

void c_guMtxConcat(const Mtx a,const Mtx b,Mtx ab) {}
void guLookAt(Mtx mt,const guVector *camPos,const guVector *camUp,const guVector *target) {}
void guPerspective(Mtx44 mt,f32 fovy,f32 aspect,f32 n,f32 f) {}
void guOrtho(Mtx44 mt,f32 t,f32 b,f32 l,f32 r,f32 n,f32 f) {}
void c_guMtxIdentity(Mtx mt) {}
void c_guMtxTransApply(const Mtx src,Mtx dst,f32 xT,f32 yT,f32 zT) {}