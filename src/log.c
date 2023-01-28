#include "log.h"
#include <stdalign.h>

_Alignas(32) FILE* log_file;

void init_log_file(void) {
    // log_file = fopen("out.log", "w");
}
void term_log_file(void) {
    // fclose(log_file);
}