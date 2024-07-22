#ifndef __CURSES_H__
#define __CURSES_H__

#if defined(_WIN32)
	#define CURSES_BGR
	#define NCURSES_MOUSE_VERSION
	#include <pdcurses.h>

#elif defined(__linux__)
	#define CURSES_RGB
	#define NCURSES_ENABLE_STDBOOL_H 0
	#include <ncurses.h>

#else
	#error "Unsupported platform"

#endif

#endif
