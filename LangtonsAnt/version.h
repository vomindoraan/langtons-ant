/**
 * @file version.h
 * App version info
 * @author vomindoraan
 */
#ifndef __VERSION_H__
#define __VERSION_H__

#define APP_NAME       "Langton's Ant"
#define APP_VERSION    "2.1"
#define APP_AUTHOR     "Konstantin Ðorđević"
#define APP_COPYRIGHT1 "(C) 2015-2025  " APP_AUTHOR
#define APP_COPYRIGHT2 "(C) 2015  Natalija Radić"
#define APP_COPYRIGHT3 "(C) 2025  Konstantinos Petrović"
#define APP_URL_REPO   "github.com/vomindoraan/langtons-ant"
#define APP_URL_ABOUT  "suluv.org/pomeraj-u-kodu-3"

#if defined(PDCURSES)
#	define APP_CURSES  "PDCurses"
#elif defined(NCURSES)
#	define APP_CURSES  "ncurses"
#endif

#endif  // __VERSION_H_
