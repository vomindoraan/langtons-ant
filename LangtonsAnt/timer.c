#include "graphics.h"

#ifdef _WIN32
#	include <Windows.h>

static double     freq_ms, freq_us;
static long long  start_ticks;

void init_timer(void)
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	freq_ms = li.QuadPart / 1.0e3;
	freq_us = li.QuadPart / 1.0e6;

	QueryPerformanceCounter(&li);
	start_ticks = li.QuadPart;
}

ttime_t get_time_ms(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (ttime_t)((li.QuadPart - start_ticks) / freq_ms);
}

ttime_t get_time_us(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (ttime_t)((li.QuadPart - start_ticks) / freq_us);
}

#else
#	include <time.h>

#endif  // _WIN32
