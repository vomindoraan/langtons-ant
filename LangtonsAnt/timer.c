#include "graphics.h"

#ifdef _WIN32
#	include <Windows.h>

static double     freq_ms, freq_us;
static long long  start_ticks;

void init_timer(void)
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	freq_ms = li.QuadPart / 1e3;
	freq_us = li.QuadPart / 1e6;

	QueryPerformanceCounter(&li);
	start_ticks = li.QuadPart;
}

ttime_t timer_millis(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (ttime_t)((li.QuadPart - start_ticks) / freq_ms);
}

ttime_t timer_micros(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (ttime_t)((li.QuadPart - start_ticks) / freq_us);
}

#else
#	include <sys/time.h>

struct timeval start;

void init_timer(void)
{
	gettimeofday(&start, 0);
}

ttime_t timer_millis(void)
{
	return timer_micros() / 1000;
}

ttime_t timer_micros(void)
{
	ttime_t ds, dus;
	struct timeval now;

	gettimeofday(&now, 0);
	ds  = now.tv_sec  - start.tv_sec;
	dus = now.tv_usec - start.tv_usec;
	return (ttime_t)(ds*1e6 + dus);
}

#endif  // _WIN32
