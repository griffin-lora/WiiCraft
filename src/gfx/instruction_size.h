#pragma once
#include <stddef.h>
#include <gctypes.h>

#define GET_BEGIN_INSTRUCTION_SIZE(VERT_COUNT) 3
#define GET_VECTOR_INSTRUCTION_SIZE(DIM, TYPE_SIZE, VERT_COUNT) ((VERT_COUNT) * (DIM) * (TYPE_SIZE))