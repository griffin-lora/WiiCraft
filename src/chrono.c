#include "chrono.h"
#include <time.h>
#include <sys/time.h>

us_t total_procedural_gen_time = 0;
us_t total_visual_gen_time = 0;
us_t last_visual_gen_time = 0;

s64 get_current_us() {
    struct timeval cur_time;
	gettimeofday(&cur_time, NULL);
	return (cur_time.tv_sec * 1000000) + cur_time.tv_usec;
}