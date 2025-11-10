/**
 * @file curses.h
 * Proxy header for pdcurses.h on Windows, ncurses.h on Linux
 * @author vomindoraan
 */
#ifndef __CURSES_H__
#define __CURSES_H__

#if defined(_WIN32)
#	define PDC_WIDE
#	define PDC_DLL_BUILD
#	define PDC_NCMOUSE
#	include <pdcurses.h>
#	ifndef PDCURSES
#		define PDCURSES  1
#	endif
#	define CURSES_BGR

#elif defined(__linux__) || defined(__APPLE__)
#	define NCURSES_WIDECHAR  1
#	include <ncurses.h>
#	ifndef NCURSES
#		define NCURSES  1
#	endif
#	define CURSES_RGB

#else
#	error "Unsupported platform"

#endif

#endif // __CURSES_H__
