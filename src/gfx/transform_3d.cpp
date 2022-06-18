#include "transform_3d.hpp"

using namespace gfx;

void gfx::set_position(transform_3d& tf, math::matrix view, f32 x, f32 y, f32 z) {
    guMtxIdentity(tf.model);
    guMtxTransApply(tf.model, tf.model, x, y, z);
    update_model_view(tf, view);
}

void gfx::load(transform_3d& tf) {
    GX_LoadPosMtxImm(tf.model_view, GX_PNMTX3);
}