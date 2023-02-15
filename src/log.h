#pragma once
#include <stdbool.h>

bool log_init(void);
void log_term(void);
__attribute__((format(printf, 1, 2))) void lprintf(const char* fmt, ...);