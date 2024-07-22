/**
 * @file io.h
 * Input/output members and function declarations
 * @author IntelligAnt
 */
#ifndef __IO_H__
#define __IO_H__

#include "logic.h"
#include "graphics.h"

/*------------------------- Input/output attributes --------------------------*/

/** Maximum filename buffer length on Windows */
#define FILENAME_BUF_LEN   256

/** Total number of fields in a Colors struct */
#define COLORS_TOTAL_FIELDS (COLOR_COUNT*2 + 4)

/** @name Bitmap file attributes */
///@{
#define BYTES_PER_PIXEL  3   // BGR
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40
///@}


/*----------------------------------------------------------------------------*
 *                                    io.c                                    *
 *----------------------------------------------------------------------------*/

/**
 * Read only color rules from file
 * @param filename Source .lant file path
 * @return Pointer to a Colors struct if successful; NULL otherwise
 * @see save_colors(char *, Colors *)
 */
Colors *load_colors(char *filename);

/**
 * Write only color rules to file
 * @param filename Destination .lant file path
 * @param colors Rules to be written
 * @return Number of written fields if successful; EOF otherwise
 * @see load_colors(char *)
 */
int save_colors(char *filename, Colors *colors);

/**
 * Read simulation state from file
 * @param filename Source .lant file path
 * @return Pointer to a Simulation struct if successful; NULL otherwise
 * @see save_simulation(char *, Simulation *)
 */
Simulation *load_simulation(char *filename);

/**
 * Write simulation state to a .lant file
 * @param filename Destination .lant file path
 * @param simulation Simulation to be written
 * @return Number of written fields if successful; EOF otherwise
 * @see load_simulation(char *)
 */
int save_simulation(char *filename, Simulation *sim);

/**
 * Save simulation grid as bitmap image
 * @param filename Destination .bmp file path
 * @param grid Grid to be written
 * @return Bitmap size if successful; EOF otherwise
 */
int save_grid_bitmap(char *filename, Grid *grid);


/*----------------------------------------------------------------------------*
 *                                  bitmap.c                                  *
 *----------------------------------------------------------------------------*/

/** Bitmap pixel type (24-bit BGR) */
typedef byte pixel_t[BYTES_PER_PIXEL];

/** Maps internal colors to bitmap-compatible pixel format */
extern const pixel_t color_map[COLOR_COUNT];

/**
 * Create bitmap file from image byte array
 * @param filename Destination .bmp file path
 * @param image Byte array in 24-bit BGR format
 * @param height Image height
 * @param width Image width
 */
int create_bitmap_file(char *filename, pixel_t *image, size_t height, size_t width);

#endif
