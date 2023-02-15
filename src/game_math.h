#pragma once
#include <gctypes.h>
#include <math.h>
#include <cglm/cglm.h>
#include <cglm/struct.h>

typedef ivec3s s32vec3s;

typedef struct {
    u8 x;
    u8 y;
    u8 z;
} u8vec3s;

typedef struct {
    s32 x;
    s32 y;
} s32vec2s;

#define LOOK_VEC(MAT) (MAT[0])
#define UP_VEC(MAT) (MAT[1])
#define RIGHT_VEC(MAT) (MAT[2])

#define IS_NON_ZERO_VEC2(VEC) (VEC.x != 0 || VEC.y != 0)
#define IS_NON_ZERO_VEC3(VEC) (VEC.x != 0 || VEC.y != 0 || VEC.z != 0)
#define LENGTH_SQUARED_VEC3(VEC) (VEC.x * VEC.x + VEC.y * VEC.y + VEC.z * VEC.z)

f32 get_eased(f32 x);

f32 get_noise_at(vec2s pos);