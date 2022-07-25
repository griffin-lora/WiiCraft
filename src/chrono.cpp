#include "chrono.hpp"
#include <ctime>
#include <sys/time.h>

using namespace chrono;

us_tp<s64> chrono::get_current_us() {
    timeval cur_time;
	gettimeofday(&cur_time, NULL);
	return (cur_time.tv_sec * 1000000) + cur_time.tv_usec;
}