#pragma once
#include <stddef.h>
#include <gctypes.h>

#define BEGIN_INSTRUCTION_SIZE 3
#define GET_VECTOR_INSTRUCTION_SIZE(DIM, TYPE_SIZE, NUM_VERTICES) ((NUM_VERTICES) * (DIM) * (TYPE_SIZE))