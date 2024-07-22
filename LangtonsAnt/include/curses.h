#if defined(_WIN32)
	#include <pdcurses.h>
#elif defined(__linux__)
	#include <ncurses.h>
#else
	#error "Unsupported platform"
#endif
