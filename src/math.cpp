#include "math.hpp"

using namespace math;

glm::mat3x3 math::from_euler_angles(f32 yaw, f32 pitch, f32 roll) {
    f32 ch = cosf(yaw);
    f32 sh = sinf(yaw);
    f32 ca = cosf(pitch);
    f32 sa = sinf(pitch);
    f32 cb = cosf(roll);
    f32 sb = sinf(roll);

    return {
        { ch * ca, sh*sb - ch*sa*cb, ch*sa*sb + sh*cb },
        { sa, ca*cb, -ca*sb },
        { -sh*ca, sh*sa*cb + ch*sb, -sh*sa*sb + ch*cb }
    };
}