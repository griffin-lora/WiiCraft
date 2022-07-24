#include <ogc/gx.h>

WGPipe pipe;

WGPipe* const wgPipe = &pipe;

GXFifoObj *GX_Init(void *base, u32 size) { return (GXFifoObj*)0x0; }
void GX_SetCopyClear(GXColor color, u32 zvalue) {}
void GX_SetViewport(f32 xOrig, f32 yOrig, f32 wd, f32 ht, f32 nearZ, f32 farZ) {}
f32 GX_GetYScaleFactor(u16 efbHeight, u16 xfbHeight) { return 1.0f; }
u32 GX_SetDispCopyYScale(f32 yscale) { return 1; }
void GX_SetScissor(u32 xOrigin, u32 yOrigin, u32 wd, u32 ht) {}
void GX_SetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {}
void GX_SetDispCopyDst(u16 wd, u16 ht) {}
void GX_SetCopyFilter(u8 aa, u8 (*sample_pattern)[2], u8 vf, u8 *vfilter) {}
void GX_SetFieldMode(u8 field_mode, u8 half_aspect_ratio) {}
void GX_SetPixelFmt(u8 pix_fmt, u8 z_fmt) {}
void GX_CopyDisp(void *dest, u8 clear) {}
void GX_SetDispCopyGamma(u8 gamma) {}

void GX_SetCullMode(u8 mode) {}
void GX_InvVtxCache(void) {}
void GX_InvalidateTexAll(void) {}

void GX_SetNumChans(u8 num) {}
void GX_SetNumTexGens(u32 nr) {}
void GX_SetTevOrder(u8 tevstage, u8 texcoord, u32 texmap, u8 color) {}
void GX_SetTevOp(u8 tevstage, u8 mode) {}
void GX_SetTexCoordGen(u16 texcoord, u32 tgen_typ, u32 tgen_src, u32 mtxsrc) {}
void GX_ClearVtxDesc(void) {}
void GX_SetVtxDesc(u8 attr, u8 type) {}
void GX_SetVtxAttrFmt(u8 vtxfmt, u32 vtxattr, u32 comptype, u32 compsize, u32 frac) {}

static u32 inp_size;

void GX_BeginDispList(void *list, u32 size) { inp_size = size; }
u32 GX_EndDispList(void) { return inp_size; }

void GX_CallDispList(void *list, u32 nbytes) {}

void GX_Begin(u8 primitve, u8 vtxfmt, u16 vtxcnt) {}

void GX_LoadPosMtxImm(f32 (*mt)[4], u32 pnidx) {}
void GX_LoadTexObj(GXTexObj *obj, u8 mapid) {}
void GX_InitTexObjFilterMode(GXTexObj *obj, u8 minfilt, u8 magfilt) {}
void GX_SetCurrentMtx(u32 mtx) {}
void GX_LoadProjectionMtx(f32 (*mt)[4], u8 type) {}
void GX_SetBlendMode(u8 type, u8 src_fact, u8 dst_fact, u8 op) {}
void GX_SetAlphaUpdate(u8 enable) {}
void GX_SetColorUpdate(u8 enable) {}
void GX_SetZMode(u8 enable, u8 func, u8 update_enable) {}
void GX_DrawDone(void) {}

void GX_SetAlphaCompare(u8 comp0, u8 ref0, u8 aop, u8 comp1, u8 ref1) {}
void GX_SetZCompLoc(u8 before_tex) {}

void GX_SetTevColor(u8 tev_regid, GXColor color) {}
void GX_SetTevColorIn(u8 tevstage, u8 a, u8 b, u8 c, u8 d) {}
void GX_SetTevAlphaIn(u8 tevstage, u8 a, u8 b, u8 c, u8 d) {}