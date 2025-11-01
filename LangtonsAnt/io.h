/**
 * @file io.h
 * Input/output members and function declarations
 * @author IntelligAnt
 */
#ifndef __IO_H__
#define __IO_H__

#include "logic.h"
#ifndef IO_NO_CURSES
#	include "curses.h"
#endif


/*------------------------- Input/output attributes --------------------------*/

/** Filename buffer size */
#define FILENAME_SZ         (size_t)256U

/** Total number of fields in a Colors struct */
#define COLORS_FIELD_COUNT  (COLOR_COUNT*2 + 4)


/*----------------------- Bitmap I/O macros and types ------------------------*/

/** @name Bitmap attributes */
///@{
#define BMP_MAX_SZ          (size_t)(1U << 28)
#define BMP_FILE_HEADER_SZ  (size_t)14U
#define BMP_INFO_HEADER_SZ  (size_t)40U
///@}

/** Pixel format size */
#define BYTES_PER_PIXEL     (size_t)3U

/** @name Pixel format conversion macros */
///@{
#define RGB_BGR(c)  (((c) & 0xA) | ((c) & 0x1) << 2 | ((c) & 0x4) >> 2)

#if defined(CURSES_RGB)
#	define RGB(c)   (c)
#	define BGR(c)   RGB_BGR(c)
#elif defined(CURSES_BGR)
#	define RGB(c)   RGB_BGR(c)
#	define BGR(c)   (c)
#endif
///@}

/** Bitmap pixel type (24-bit BGR/RGB) */
typedef byte  pixel_t[BYTES_PER_PIXEL];

/**
 * Maps internal colors to bitmap-compatible pixel type
 * Index with @ref BGR(c) or @ref RGB(c) to explicitly convert format
 */
extern const pixel_t  color_map[COLOR_COUNT];


/*----------------------------------------------------------------------------*
 *                                    io.c                                    *
 *----------------------------------------------------------------------------*/

/**
 * Read only color rules from file
 * @param filename Source .lant file path
 * @return Pointer to a Colors struct if successful; NULL otherwise
 * @see save_colors(const char *, Colors *)
 */
Colors *load_colors(const char *filename);

/**
 * Write only color rules to file
 * @param filename Destination .lant file path
 * @param colors Rules to be written
 * @return Number of written fields if successful; EOF otherwise
 * @see load_colors(const char *)
 */
int save_colors(const char *filename, Colors *colors);

/**
 * Read simulation state from file
 * @param filename Source .lant file path
 * @return Pointer to a Simulation struct if successful; NULL otherwise
 * @see save_simulation(const char *, Simulation *)
 */
Simulation *load_simulation(const char *filename);

/**
 * Write simulation state to a .lant file
 * @param filename Destination .lant file path
 * @param simulation Simulation to be written
 * @return Number of written fields if successful; EOF otherwise
 * @see load_simulation(const char *)
 */
int save_simulation(const char *filename, Simulation *sim);

/**
 * Save simulation grid as bitmap image
 * @param filename Destination .bmp file path
 * @param grid Grid to be written
 * @return Bitmap size if successful; EOF otherwise
 * @see create_bitmap_file(const char *, pixel_t *, size_t, size_t)
 */
int save_grid_bitmap(const char *filename, Grid *grid);


/*----------------------------------------------------------------------------*
 *                                bitmap_io.c                                 *
 *----------------------------------------------------------------------------*/

/**
 * Create bitmap file from image byte array
 * @param filename Destination .bmp file path
 * @param image Byte array in 24-bit BGR format
 * @param height Image height
 * @param width Image width
 */
int create_bitmap_file(const char *filename, pixel_t *image, size_t height, size_t width);

#endif  // __IO_H__
