#include "log.h"
#include <stdalign.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define LOG_SIZE (0x400 - 4)

_Alignas(32) static struct {
    size_t index;
    char buf[LOG_SIZE];
} log_info;

void log_init(void) {
}
void log_term(void) {
}

void lprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_info.index += vsnprintf(&log_info.buf[log_info.index], LOG_SIZE - log_info.index, fmt, args);
    va_end(args);
}