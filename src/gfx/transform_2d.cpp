#include "transform_2d.hpp"

using namespace gfx;

void gfx::set_position(transform_2d& tf, f32 x, f32 y) {
    guMtxIdentity(tf.model_view);
    guMtxTransApply(tf.model_view, tf.model_view, x, y, 0.0f);
}

void gfx::load(transform_2d& tf) {
    GX_LoadPosMtxImm(tf.model_view, GX_PNMTX3);
}