#pragma once
#include <stdio.h>

extern FILE* log_file;

void init_log_file(void);
void term_log_file(void);