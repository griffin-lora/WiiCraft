#pragma once
#include <gctypes.h>
#include <math.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/types-struct.h>
#include <cglm/ivec3.h>
#include <cglm/ivec2.h>

typedef ivec3s s32vec3s;
typedef ivec2s s32vec2s;

typedef struct {
    u8 x;
    u8 y;
    u8 z;
} u8vec3s;

#define LOOK_VEC(MAT) (MAT[0])
#define UP_VEC(MAT) (MAT[1])
#define RIGHT_VEC(MAT) (MAT[2])

#define IS_NON_ZERO_VEC2(VEC) (VEC.x != 0 || VEC.y != 0)
#define IS_NON_ZERO_VEC3(VEC) (VEC.x != 0 || VEC.y != 0 || VEC.z != 0)
#define LENGTH_SQUARED_VEC3(VEC) (VEC.x * VEC.x + VEC.y * VEC.y + VEC.z * VEC.z)

f32 get_eased(f32 x);

f32 lerpf(f32 min, f32 max, f32 alpha);

f32 get_noise_at(vec2s pos);