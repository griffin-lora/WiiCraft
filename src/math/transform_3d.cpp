#include "transform_3d.hpp"

using namespace math;

transform_3d::transform_3d(transform_3d&& other) {
    for (u8 i = 0; i < 3; i++) {
        for (u8 j = 0; j < 4; j++) {
            model[i][j] = other.model[i][j];
            model_view[i][j] = other.model_view[i][j];
            other.model[i][j] = 0;
            other.model_view[i][j] = 0;
        }
    }
}

void transform_3d::set_position(matrix view, f32 x, f32 y, f32 z) {
    guMtxIdentity(model);
    guMtxTransApply(model, model, x, y, z);
    update_model_view(view);
}