#include "position_state.hpp"

using namespace gfx;

void gfx::set_position(position_state& ps, math::matrix view, f32 x, f32 y, f32 z) {
    guMtxIdentity(ps.model);
    guMtxTransApply(ps.model, ps.model, x, y, z);
    update_model_view(ps, view);
}

void gfx::load(position_state& ps) {
    GX_LoadPosMtxImm(ps.model_view, GX_PNMTX3);
    
    GX_SetCurrentMtx(GX_PNMTX3);
}