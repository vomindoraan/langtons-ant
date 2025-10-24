/**
 * @file curses.h
 * Proxy header for pdcurses.h on Windows, ncurses.h on Linux
 * @author vomindoraan
 */
#ifndef __CURSES_H__
#define __CURSES_H__

#if defined(_WIN32)
#	define CURSES_BGR
#	define NCURSES_MOUSE_VERSION
#	include <pdcurses.h>
#	ifndef PDCURSES
#		define PDCURSES
#	endif

#elif defined(__linux__)
#	define CURSES_RGB
#	define NCURSES_ENABLE_STDBOOL_H  0
#	include <ncurses.h>
#	ifndef NCURSES
#		define NCURSES
#	endif

#else
#	error "Unsupported platform"

#endif

#endif // __CURSES_H__
