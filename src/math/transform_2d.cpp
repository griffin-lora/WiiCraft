#include "transform_2d.hpp"

using namespace math;

void transform_2d::set_position(f32 x, f32 y) {
    guMtxIdentity(model_view);
    guMtxTransApply(model_view, model_view, x, y, 0.0f);
}