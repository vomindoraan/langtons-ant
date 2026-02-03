/**
 * @file logic.h
 * Logic members and function declarations
 * @author IntelligAnt
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>


/*--------------------- General purpose macros and types ---------------------*/

/** Static array length macro */
#define LEN(a)         (sizeof(a) / sizeof(*a))

///@{
/** Stringify macro value macros */
#define STR(x)         STR_(x)
#define STR_(x)        #x
///@}

///@{
/** Standard max/min macros */
#define MAX(x, y)      (((x) > (y)) ? (x) : (y))
#define MIN(x, y)      (((x) < (y)) ? (x) : (y))
///@}

/** Standard sign macro */
#define SGN(x)         (((x) > 0) - ((x) < 0))

/** Standard square macro */
#define SQ(x)          ((x) * (x))

/** Integer ceiling division macro */
#define CDIV(x, y)     (((x) + (y) - 1) / (y))

/** Linear interpolation macro */
#define LERP(a, b, t)  ((a) * (1.0-(t)) + (b) * (t))

/** Global byte type */
typedef unsigned char  byte;


/*------------------------- Vector macros and types --------------------------*/

/** Vector constants */
///@{
#define VECTOR_ZERO        (Vector2i) { 0, 0 }
#define VECTOR_INVALID     (Vector2i) { INT_MIN, INT_MIN }
///@}

/** Equality comparison macro for two vectors */
#define VECTOR_EQ(v1, v2)  ((v1).y == (v2).y && (v1).x == (v2).x)

/** Vector container (int y, int x) */
typedef struct vector2i {
	int  y, x;  /**< Coordinates */
} Vector2i;


/*--------------------------- Ant type definitions ---------------------------*/

/** Ant directions enum */
typedef enum {
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT
} Direction;

/** Ant container */
typedef struct ant {
	Vector2i   pos;  /**< Current position */
	Direction  dir;  /**< Direction the ant is facing */
} Ant;


/*----------------------- Color rules macros and types -----------------------*/

/** @name Colors struct constants */
///@{
#define COLOR_COUNT  16
#define COLOR_NONE   -1

#define TURN_LEFT    -1
#define TURN_NONE    0
#define TURN_RIGHT   1
///@}

/** @name Colors utility macros */
///@{
#define COLOR_NEXT(cs, c)  (cs)->next[c]
#define COLOR_TURN(cs, c)  (cs)->turn[c]

#define TURN_CHAR(t)       (((t) == TURN_LEFT)  ? '<' :       \
                            ((t) == TURN_RIGHT) ? '>' : '-')
///@}

/** Curses color type */
typedef short        color_t;

/** Turn direction for given rule */
typedef signed char  turn_t;

/** Color rules container */
typedef struct colors {
	color_t   next[COLOR_COUNT];
	turn_t    turn[COLOR_COUNT];
	color_t   first, last, def;
	unsigned  n;
} Colors;  // TODO: Finish logic docs & add @see


/*-------------------------- Grid macros and types ---------------------------*/

/** @name Grid struct constants */
///@{
#define GRID_MULT                3
#define GRID_SIZE_THRESHOLD      19682  // 3^9 - 1
#define GRID_USAGE_THRESHOLD     0.5
#define GRID_DEF_INIT_SIZE       4
#define GRID_MAX_INIT_SIZE       7
#define GRID_MIN_INIT_SIZE       2
#define GRID_MAX_SILENT_EXPAND   (GRID_SIZE_THRESHOLD + 1)  // TODO: Add a dynamic silent expand step

#define GRID_SIZE_SMALL(g)       (g)->init_size  // 2, 3, 4, 5, 6, 7
#define GRID_SIZE_MEDIUM(g)      (GRID_SIZE_SMALL(g) * GRID_MULT)
#define GRID_SIZE_LARGE(g)       (GRID_SIZE_MEDIUM(g) * GRID_MULT)
#define IS_GRID_LARGE(g)         ((g)->size >= GRID_SIZE_LARGE(g))
#define GRID_EFFICIENCY(g)       (SQ((g)->size) / ((g)->colored * (double)sizeof(SparseCell)))
#define GRID_COLOR_AT(g, p)      (is_grid_sparse(g) ? sparse_color_at(g, p) : (g)->c[(p).y][(p).x])
#define GRID_ANT_COLOR(g, a)     GRID_COLOR_AT(g, (a)->pos)
///@}

/** @name Sparse matrix bit packing macros */
///@{
#define CSR_COLOR_MASK           (0xF << 28)
#define CSR_GET_COLOR(sc)        (((sc)->packed & CSR_COLOR_MASK) >> 28)
#define CSR_SET_COLOR(sc, col)   ((sc)->packed = ((sc)->packed & ~CSR_COLOR_MASK) | ((col) << 28))
#define CSR_GET_COLUMN(sc)       ((sc)->packed & ~CSR_COLOR_MASK)
#define CSR_SET_COLUMN(sc, col)  ((sc)->packed = ((sc)->packed &  CSR_COLOR_MASK) | ((col) & ~CSR_COLOR_MASK))
///@}

/** Sparse matrix cell container */
typedef struct cell {
	unsigned     packed;
	struct cell *next;
} SparseCell;

/** Grid container */
typedef struct grid {
	byte       **c, **tmp;
	byte         def_color;
	SparseCell **csr;
	unsigned     init_size, size, tmp_size;
	unsigned     colored;
	Vector2i     pos_tl, pos_br;
} Grid;


/*------------------------ Simulation type definition ------------------------*/

/** Simulation container */
typedef struct simulation {
	Colors   *colors;
	Grid     *grid;
	Ant      *ant;
	unsigned  steps;
	bool      is_running;
} Simulation;


/*----------------------------------------------------------------------------*
 *                                   ant.c                                    *
 *----------------------------------------------------------------------------*/

Ant *ant_new(Grid *grid, Direction dir);
void ant_delete(Ant *ant);
bool ant_move(Ant *ant, Grid *grid, Colors *colors);
bool is_ant_in_bounds(Ant *ant, Grid *grid);


/*----------------------------------------------------------------------------*
 *                                  colors.c                                  *
 *----------------------------------------------------------------------------*/

Colors *colors_new(color_t def);
void colors_delete(Colors *colors);
void colors_push(Colors *colors, color_t c, turn_t turn);
void colors_pop(Colors *colors, color_t c);
void colors_clear(Colors *colors);
color_t colors_at(Colors *colors, unsigned index);
void colors_update(Colors *colors, unsigned index, color_t c, turn_t turn);
void colors_set_turn(Colors *colors, unsigned index, turn_t turn);
bool color_exists(Colors *colors, color_t c);
bool is_color_special(Colors *colors, color_t c);
bool is_colors_empty(Colors *colors);
bool has_enough_colors(Colors *colors);


/*----------------------------------------------------------------------------*
 *                                   grid.c                                   *
 *----------------------------------------------------------------------------*/

Grid *grid_new(Colors *colors, unsigned init_size);
void grid_delete(Grid *grid);
void grid_silent_expand(Grid *grid);
void grid_expand(Grid *grid, Ant *ant);
void grid_make_sparse(Grid *grid);
bool is_grid_sparse(Grid *grid);
bool is_grid_usage_low(Grid *grid);
void sparse_prepend(SparseCell **phead, unsigned column, byte color);
SparseCell *sparse_append(SparseCell *head, unsigned column, byte color);
byte sparse_color_at(Grid *grid, Vector2i pos);


/*----------------------------------------------------------------------------*
 *                                simulation.c                                *
 *----------------------------------------------------------------------------*/

Simulation *simulation_new(Colors *colors, unsigned init_size);
void simulation_delete(Simulation *sim);
void simulation_run(Simulation *sim);
void simulation_halt(Simulation *sim);
bool simulation_step(Simulation *sim);
bool is_simulation_running(Simulation *sim);
bool has_simulation_started(Simulation *sim);

#endif  // __LOGIC_H__
