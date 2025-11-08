/**
 * @file graphics.h
 * Graphics members and function declarations
 * @author IntelligAnt
 */
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "logic.h"
#include "sprites.h"
#include "curses.h"

#include <math.h>

/** @name Compile flags */
///@{
/** Should save button in menu be drawn and active? */
#if defined(SAVE_ENABLED)
#	define SAVE_ENABLE   1
#elif !defined(SAVE_ENABLE)
#	define SAVE_ENABLE   1
#endif

/** Should load/save be limited to a single file? (e.g. no keyboard) */
#if defined(GALLERY_MODE_ON)
#	define GALLERY_MODE  1
#elif !defined(GALLERY_MODE)
#	define GALLERY_MODE  0
#endif
///@}


/*------------------------ Character graphics macros -------------------------*/

/** @name Preferred console font settings */
///@{
#define CONSOLE_FONT_NAME  L"PxPlus IBM EGA 8x8"
#define CONSOLE_FONT_FILE  L"PxPlus_IBM_EGA_8x8.ttf"
#define CONSOLE_FONT_SIZE  7
///@}

/** Empty character used for drawing */
#define CHAR_EMPTY       ' '

/** Full character used for drawing */
#define CHAR_FULL        (' ' | A_REVERSE)
//#define CHAR_FULL        ACS_BLOCK

/** Semi-visible on the UI background */
#define CHAR_SEMI        (ui_pair | ACS_CKBOARD)

/** Guaranteed to be visible on the UI background */
#define CHAR_VISIBLE(c)  (((c) == COLOR_FOR(bg_pair)) ? CHAR_SEMI : PAIR_FOR(c) | CHAR_FULL)


/*--------------------------- Display color macros ---------------------------*/

#undef COLOR_BLACK
#undef COLOR_RED
#undef COLOR_GREEN
#undef COLOR_BLUE
#undef COLOR_CYAN
#undef COLOR_MAGENTA
#undef COLOR_YELLOW
#undef COLOR_WHITE

/** @name 4-bit color macros (HTML names) */
///@{
#define COLOR_BLACK      0
#define COLOR_SILVER     7

#define COLOR_GRAY       8
#define COLOR_WHITE      15

#ifdef CURSES_RGB
#	define COLOR_MAROON  1
#	define COLOR_GREEN   2
#	define COLOR_NAVY    4

#	define COLOR_RED     9
#	define COLOR_LIME    10
#	define COLOR_BLUE    12
#else
#	define COLOR_NAVY    1
#	define COLOR_GREEN   2
#	define COLOR_MAROON  4

#	define COLOR_BLUE    9
#	define COLOR_LIME    10
#	define COLOR_RED     12
#endif

#define COLOR_TEAL       (COLOR_NAVY | COLOR_GREEN)
#define COLOR_PURPLE     (COLOR_MAROON | COLOR_NAVY)
#define COLOR_OLIVE      (COLOR_MAROON | COLOR_GREEN)

#define COLOR_AQUA       (COLOR_BLUE | COLOR_LIME)
#define COLOR_FUCHSIA    (COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW     (COLOR_RED | COLOR_LIME)
///@}

/** @name Utility macros for colors */
///@{
#define PAIRNO_FOR(c)                ((c) + 1)
#define PAIR_FOR(c)                  COLOR_PAIR(PAIRNO_FOR(c))
#define COLOR_FOR(p)                 (color_t)(PAIR_NUMBER(p) - 1)
#define AVAILABLE_COLOR(def, c, bk)  (((def) == (c)) ? (bk) : (c))
#define AVAILABLE_PAIR(def, c, bk)   PAIR_FOR(AVAILABLE_COLOR(def, c, bk))
#define IS_COLOR_BRIGHT(c)           ((c) == COLOR_SILVER || ((c) > 8 && (c) != COLOR_BLUE))
///@}


/*----------------------- Grid window macros and types -----------------------*/

/** @name Grid window attributes */
///@{
#define GRID_WINDOW_SIZE    109
#define GRID_VIEW_SIZE      (GRID_WINDOW_SIZE - 1)  // TODO: Variable scrollbar size
#define LINE_WIDTH_SMALL    2
#define LINE_WIDTH_MEDIUM   1
#define LINE_WIDTH_LARGE    0
#define SCROLL_STEP_SMALL   (GRID_VIEW_SIZE / 18)
#define SCROLL_STEP_MEDIUM  (GRID_VIEW_SIZE / 4)
#define SCROLL_STEP_LARGE   GRID_VIEW_SIZE
///@}

/** @name Generic size calculations for grid functions */
///@{
#define CELL_SIZE(gs, lw)       ((GRID_WINDOW_SIZE-(lw)) / (gs) - (lw))
#define TOTAL_SIZE(gs, lw, cs)  (((gs)+1)*(lw) + (gs)*(cs))
#define OFFSET_SIZE(t)          ((GRID_WINDOW_SIZE-(t)) / 2)
///@}

/** @name Scroll utility macros */
///@{
#define ORIGIN_COORD(gs, vs, sc)      ((gs)/2 - (vs)/2 + (sc))
#define ORIGIN_POS(gs, vs, scy, scx)  (Vector2i) { ORIGIN_COORD(gs, vs, scy), ORIGIN_COORD(gs, vs, scx) }
///@}

/** Structure for managing scroll data */
typedef struct scroll_info {
	bool    enabled;           /**< Scrolling is enabled */                     /**@{*/
	int     y, x;              /**< Current view position relative to (0,0) */  /**@}*/ /**@{*/
	int     hcenter, vcenter;  /**< Scrollbar slider positions */               /**@}*/
	double  scale;             /**< Scaling multiplier */
} ScrollInfo;


/*----------------------- Menu window macros and types -----------------------*/

/** @name Menu window attributes */
///@{
#define MENU_WINDOW_WIDTH    44
#define MENU_WINDOW_HEIGHT   GRID_WINDOW_SIZE
#define MENU_H_MARGIN        3
#define MENU_V_MARGIN        3
#define MENU_H_PAD           2
#define MENU_V_PAD           4
#define MENU_COL_WIDTH       16
#define MENU_LEFT_COL_X      MENU_H_MARGIN
#define MENU_RIGHT_COL_X     (MENU_WINDOW_WIDTH - MENU_H_MARGIN - MENU_COL_WIDTH)
#define MENU_LEFT_COL_Y      (MENU_LOGO_Y + MENU_LOGO_HEIGHT + MENU_V_PAD - 1)
#define MENU_RIGHT_COL_Y     MENU_LEFT_COL_Y
#define MENU_LOGO_WIDTH      SPRITE_LOGO_WIDTH
#define MENU_LOGO_HEIGHT     SPRITE_LOGO_HEIGHT
#define MENU_LOGO_Y          MENU_V_MARGIN
#define MENU_RULES_Y         MENU_LEFT_COL_Y
#define MENU_INIT_SIZE_Y     MENU_RIGHT_COL_Y
#define MENU_DIRECTION_Y     (MENU_INIT_SIZE_Y + MENU_V_PAD + 7)
#define MENU_STEPUP_Y        (MENU_DIRECTION_Y + MENU_V_PAD + 9)
#define MENU_SPEED_Y         (MENU_STEPUP_Y + MENU_V_PAD + 5)
#define MENU_SPEED_HEIGHT    9  // n*8 + 1
#define MENU_STATE_FUNC_Y    (MENU_SPEED_Y + MENU_SPEED_HEIGHT + MENU_V_PAD + 6)
#define MENU_GRID_SIZE_Y     (MENU_STATE_FUNC_Y + MENU_V_PAD + 4)
#define MENU_STATUS_Y        (MENU_WINDOW_HEIGHT - MENU_V_MARGIN - 7)
#define MENU_STEPS_LEN       8
#define MENU_BORDER_COLOR    COLOR_NAVY
#define MENU_BORDER_COLOR_S  COLOR_PURPLE
#define MENU_ACTIVE_COLOR    COLOR_BLUE
#define MENU_INACTIVE_COLOR  COLOR_GRAY
///@}

/** @name Menu button attributes */
///@{
#define MENU_CONTROLS_Y      (MENU_STATUS_Y - MENU_V_PAD - MENU_BUTTON_HEIGHT + 1)
#define MENU_BUTTON_WIDTH    11
#define MENU_BUTTON_HEIGHT   7
#define MENU_BUTTON_PWIDTH   (MENU_BUTTON_WIDTH + MENU_H_PAD)
#define MENU_BUTTON_PHEIGHT  (MENU_BUTTON_HEIGHT + MENU_H_PAD)
#define MENU_PLAY_X          MENU_LEFT_COL_X
#define MENU_STOP_X          (MENU_LEFT_COL_X + MENU_BUTTON_PWIDTH)
#define MENU_LOAD_X          (MENU_STOP_X + MENU_BUTTON_PWIDTH)
#if SAVE_ENABLE
#	define MENU_SAVE_X       MENU_LOAD_X
#endif
#define MENU_PLAY_COLOR      COLOR_GREEN
#define MENU_PAUSE_COLOR     COLOR_YELLOW
#define MENU_STOP_COLOR      COLOR_RED
#define MENU_CLEAR_COLOR     COLOR_TEAL

#define MENU_UDARROW_WIDTH   SPRITE_UDARROW_WIDTH
#define MENU_UDARROW_HEIGHT  SPRITE_UDARROW_HEIGHT
#define MENU_RLARROW_WIDTH   SPRITE_RLARROW_WIDTH
#define MENU_RLARROW_HEIGHT  SPRITE_RLARROW_HEIGHT
#define MENU_STEPUP_SIZE     SPRITE_STEPUP_SIZE
///@}

/** @name Menu color tiles attributes */
///@{
#define MENU_TILE_SIZE       7
#define MENU_TILE_H_PAD      3
#define MENU_TILE_V_PAD      2
#define MENU_TILE_PWIDTH     (MENU_TILE_SIZE + MENU_TILE_H_PAD)
#define MENU_TILE_PHEIGHT    (MENU_TILE_SIZE + MENU_TILE_V_PAD)
#define MENU_TILES_COLS      2
#define MENU_TILES_PER_COL   7
#define MENU_TILES_COUNT     (MENU_TILES_COLS * MENU_TILES_PER_COL)
#define MENU_TILES_WIDTH     ((MENU_TILES_COLS-1)*MENU_TILE_PWIDTH + MENU_TILE_SIZE)
#define MENU_TILES_HEIGHT    (MENU_TILES_PER_COL*MENU_TILE_PHEIGHT + MENU_TILE_V_PAD + 3)
///@}

/** Currently active menu settings */
typedef struct settings {
	Colors     *colors;      /**< Color rules */
	unsigned    init_size;   /**< Initial grid size */
	unsigned    speed;       /**< Speed multiplier */
	Simulation *simulation;  /**< Active simulation */
} Settings;

/** Status indicator type for I/O operations in the menu */
typedef enum {
	STATUS_NONE,
	STATUS_SUCCESS,
	STATUS_FAILURE,
	STATUS_PENDING
} IOStatus;


/*---------------------- Dialog window macros and types ----------------------*/

/** @name Dialog window attributes */
///@{
#define DIALOG_TILE_SIZE      3
#define DIALOG_TILE_ROWS      3
#define DIALOG_TILE_COLS      5
#define DIALOG_BUTTON_WIDTH   7
#define DIALOG_BUTTON_HEIGHT  3
#define DIALOG_DELETE_WIDTH   5
#define DIALOG_DELETE_HEIGHT  DIALOG_BUTTON_HEIGHT
#define DIALOG_DELETE_COLOR   COLOR_RED
#define DIALOG_WINDOW_WIDTH   (DIALOG_TILE_COLS*DIALOG_TILE_SIZE + 2)
#define DIALOG_WINDOW_HEIGHT  (DIALOG_TILE_ROWS*DIALOG_TILE_SIZE + DIALOG_BUTTON_HEIGHT*2 + 4)
///@}

/** Designates which colors are to be set in the dialog */
///@{
#define CIDX_NEWCOLOR  -1
#define CIDX_DEFAULT   -2
///@}


/*--------------------- Input handling macros and types ----------------------*/

/** Escape key literal for input handling */
#define KEY_ESC  0x1B

/** Should react on key press instead of release (click)? */
#define MOUSE_ACT_ON_PRESS  true

/** @name Mouse button event flags */
///@{
#if MOUSE_ACT_ON_PRESS
#	define MOUSE_LB_EVENT  BUTTON1_PRESSED
#	define MOUSE_RB_EVENT  BUTTON3_PRESSED
	// WARN: ncurses mouse handling breaks if mouseinterval(0) is set and only
	//       *_PRESSED is selected without *_RELEASED or *_CLICKED
#	define MOUSE_MASK      (BUTTON1_PRESSED  | BUTTON3_PRESSED  | MOUSE_ANTIMASK)
#	define MOUSE_ANTIMASK  (BUTTON1_RELEASED | BUTTON3_RELEASED)
#else
#	define MOUSE_LB_EVENT  BUTTON1_CLICKED
#	define MOUSE_RB_EVENT  BUTTON3_CLICKED
#	define MOUSE_MASK      (BUTTON1_CLICKED | BUTTON3_CLICKED)
#endif
///@}

/** @name Window state change flags */
///@{
#define STATE_NO_CHANGE       0
#define STATE_GRID_CHANGED    1
#define STATE_MENU_CHANGED    2
#define STATE_COLORS_CHANGED  4
///@}

/** Window state change as a result of input events (bitwise OR of STATE_* fields) */
typedef byte     state_t;

/** Pending action function pointer */
typedef state_t  (*pending_func_t)(void *);

/** Structure for scheduling actions to be processed on the next frame */
typedef struct pending_action {
	pending_func_t  func;  /**< Function pointer to action */
	void           *arg;   /**< Generic pointer to argument (must be downcasted in func) */
} PendingAction;


/*-------------------- Loop performance macros and types ---------------------*/

/** @name Performance settings */
///@{
#define LOOP_DEF_SPEED          2     /**< Default speed multiplier */
#define LOOP_MIN_SPEED          1     /**< Minimum allowed speed multiplier */
#define LOOP_MAX_SPEED          9     /**< Maximum allowed speed multiplier */
#define LOOP_MIN_STEP_TIME_S    1e-5  /**< Min time per step (max speed), > 0 */
#define LOOP_MAX_STEP_TIME_S    0.75  /**< Max time per step (min speed) */
#define LOOP_FRAMES_PER_S       30    /**< Target framerate for drawing */
///@}

/** @name Timestep calculation macros */
///@{
#define LOOP_FRAME_TIME_MS      (1e3 / LOOP_FRAMES_PER_S)
#define LOOP_FRAME_TIME_US      (1e6 / LOOP_FRAMES_PER_S)
#define LOOP_STEP_TIME_MS(s)    (1e3 * LOOP_STEP_TIME_S(s))
#define LOOP_STEP_TIME_US(s)    (1e6 * LOOP_STEP_TIME_S(s))
#define LOOP_STEP_TIME_S(s)     LOOP_EASE(LOOP_MAX_STEP_TIME_S, LOOP_MIN_STEP_TIME_S, LOOP_SPEED_COEF(s))
#define LOOP_SPEED_COEF(s)      (((double)(s) - LOOP_MIN_SPEED) / (LOOP_MAX_SPEED - LOOP_MIN_SPEED))
///@}

/** @name Interpolation/easing macros */
///@{
#define EASE_LIN(a, b, t)       LERP(a, b, t)
#define EASE_LOG(a, b, t)       (pow(a, 1.0-(t)) * pow(b, t))
#define EASE_IN_QUAD(a, b, t)   ((a) + ((b)-(a)) * SQ(t))
#define EASE_OUT_QUAD(a, b, t)  ((a) + ((b)-(a)) * (2.0*(t) - SQ(t)))
#ifndef LOOP_EASE
#	define LOOP_EASE            EASE_LOG
#endif
///@}

/** Timer (delta) time type (ms/us) */
typedef long long  ttime_t;


/*------------------------ Global variables/constants ------------------------*/

/** @name Globals */
///@{
extern chtype          fg_pair, bg_pair;
extern chtype          ui_pair, ui_text_pair;

extern WINDOW         *gridw;
extern ScrollInfo      gridscrl;
extern const Vector2i  grid_pos;

extern WINDOW         *menuw;
extern Settings        stgs;
extern IOStatus        load_status, save_status;
extern PendingAction   pending_action;
extern const Vector2i  menu_pos;
extern const Vector2i  menu_logo_pos;
extern const Vector2i  menu_isize_u_pos, menu_isize_d_pos;
extern const Vector2i  menu_dir_u_pos, menu_dir_r_pos, menu_dir_d_pos, menu_dir_l_pos;
extern const Vector2i  menu_stepup_pos;
extern const Vector2i  menu_speed_u_pos, menu_speed_d_pos;
extern const Vector2i  menu_play_pos, menu_stop_pos;
extern const Vector2i  menu_load_pos;
#if SAVE_ENABLE
extern const Vector2i  menu_save_pos;
#endif

extern WINDOW         *dialogw;
extern Vector2i        dialog_pos;
extern const char     *dialog_cdef_msg;
///@}


/*----------------------------------------------------------------------------*
 *                                 graphics.c                                 *
 *----------------------------------------------------------------------------*/

/**
 * Initializes all color pairs (c[0..15], bg_color)
 * @param fg_color Foreground color
 * @param bg_color Background color
 */
void init_def_pairs(color_t fg_color, color_t bg_color);

/**
 * Initializes graphics and all windows
 * @param fg_color Foreground color
 * @param bg_color Background color
 * @see end_graphics(void)
 */
void init_graphics(color_t fg_color, color_t bg_color);

/**
 * Closes windows and ends drawing
 * @see init_graphics(color_t, color_t)
 */
void end_graphics(void);

/**
* Converts relative coordinates into absolute
* @param rel Relative vector
* @param origin Point of reference
* @return Absolute vector
* @see abs2rel(Vector2i, Vector2i)
*/
Vector2i rel2abs(Vector2i rel, Vector2i origin);

/**
* Converts absolute coordinates into relative
* @param abs Absolute vector
* @param origin Point of reference
* @return Relative vector
* @see rel2abs(Vector2i, Vector2i)
*/
Vector2i abs2rel(Vector2i abs, Vector2i origin);

/**
* Checks if a vector is contained within an area
* @param top_left Area origin
* @param width Area width
* @param height Area height
* @param v Vector to be checked
* @return Does area contain the vector?
*/
bool area_contains(Vector2i top_left, unsigned width, unsigned height, Vector2i v);

/**
 * Utility function for drawing square boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param size Box size
 * @see draw_rect(WINDOW *, Vector2i, unsigned, unsigned)
 */
void draw_square(WINDOW *w, Vector2i top_left, unsigned size);

/**
 * Utility function for drawing rectangular boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param width Box width
 * @param height Box height
 * @see draw_square(WINDOW *, Vector2i, unsigned)
 */
void draw_rect(WINDOW *w, Vector2i top_left, unsigned width, unsigned height);

/**
 * Utility function for drawing thin rectangular frames
 * @param w Window to draw to
 * @param top_left Box origin
 * @param width Box width
 * @param height Box height
 */
void draw_frame(WINDOW *w, Vector2i top_left, unsigned width, unsigned height);

/**
 * Utility function for drawing monochrome sprites
 * @param w Window to draw to
 * @param sprite Sprite to be drawn and its size as SpriteInfo
 * @param top_left Sprite origin
 */
void draw_sprite(WINDOW *w, SpriteInfo sprite, Vector2i top_left);

/**
 * Converts a direction into its char representation
 * @param dir Direction
 * @return Appropriate arrow char for the direction
 * @see turn2arrow(turn_t)
 */
chtype dir2arrow(Direction dir);

/**
 * Converts a turn direction into its char representation
 * @param turn Turn direction
 * @return Appropriate arrow char for the turn direction
 * @see dir2arrow(Direction)
 */
chtype turn2arrow(turn_t turn);


/*----------------------------------------------------------------------------*
 *                                main_loop.c                                 *
 *----------------------------------------------------------------------------*/

/**
 * Main draw/update loop for the current simulation
 * @see stop_main_loop(void)
 * @see simulation_step(Simulation *)
 */
void main_loop(void);

/**
 * Stops the main draw/update loop
 * @see main_loop(void)
 */
void stop_main_loop(void);


/*----------------------------------------------------------------------------*
 *                                  timer.c                                   *
 *----------------------------------------------------------------------------*/

// TODO: Write docs
void init_timer(void);
ttime_t timer_millis(void);
ttime_t timer_micros(void);


/*----------------------------------------------------------------------------*
 *                               grid_window.c                                *
 *----------------------------------------------------------------------------*/

/**
 * Initializes grid window and related components
 * @see end_grid_window(void)
 */
void init_grid_window(void);

/**
 * Closes grid window and ends drawing
 * @see init_grid_window(void)
 */
void end_grid_window(void);

/**
 * Draws the entire grid (the portion shown by gridscrl)
 * @param grid Grid from which to draw (NULL for empty window with no grid)
 * @param ant Ant to be drawn in the grid (NULL for no ant)
 * @see draw_grid_iter(Grid *, Vector2i)
 */
void draw_grid_full(Grid *grid, Ant *ant);

/**
 * Draws the given cell in the grid (the portion shown by gridscrl)
 * Suitable for calling in loops as it does less work than draw_grid__full
 * @param grid Grid from which to draw
 * @param ant Ant to be drawn in the grid (NULL for no ant)
 * @param prev_pos Position of cell that has changed and should be drawn
 * @see draw_grid_full(Grid *)
 */
void draw_grid_iter(Grid *grid, Ant *ant, Vector2i prev_pos);

/**
 * Scrolls the grid relative to the current gridscrl position
 * @param grid Grid from which to draw
 * @param dy Relative y offset
 * @param dx Relative x offset
 * @see scroll_set(Grid *, int, int)
 * @see scroll_reset(void)
 */
void scroll_by(Grid *grid, int dy, int dx);

/**
 * Sets the absolute gridscrl position on the grid
 * @param grid Grid from which to draw
 * @param y Absolute y offset
 * @param x Absolute x offset
 * @see scroll_by(Grid, int, int)
 * @see scroll_reset(void)
 */
void scroll_set(Grid *grid, int y, int x);

/**
 * Resets gridscrl to its initial state
 * @see scroll_by(Grid *, int, int)
 * @see scroll_set(Grid *, int, int)
 */
void scroll_reset(void);


/*----------------------------------------------------------------------------*
 *                              grid_controls.c                               *
 *----------------------------------------------------------------------------*/

/**
 * Handles key commands passed to the grid window
 * @param grid Grid to be acted upon
 * @param ant Ant to be acted upon
 * @param key Key that was pressed
 * @param mouse Pointer to mouse event if one happened; NULL otherwise
 * @return STATE_GRID_CHANGED if grid changed; STATE_NO_CHANGE otherwise
 * @see grid_mouse_command(Grid *)
 */
state_t grid_key_command(Grid *grid, Ant *ant, int key, MEVENT *mouse);

/**
 * Handles mouse commands passed to the grid window
 * @param grid Grid to be acted upon
 * @param ant Ant to be acted upon
 * @param mouse Pointer to mouse event if one happened; NULL otherwise
 * @return STATE_GRID_CHANGED if grid changed; STATE_NO_CHANGE otherwise
 * @see grid_key_command(Grid *, Ant *, int)
 */
state_t grid_mouse_command(Grid *grid, Ant *ant, MEVENT *mouse);


/*----------------------------------------------------------------------------*
 *                               menu_window.c                                *
 *----------------------------------------------------------------------------*/

/**
 * Initializes menu window and related components
 * @see end_menu_window(void)
 */
void init_menu_window(void);

/**
 * Closes menu window and ends drawing
 * @see init_menu_window(void)
 */
void end_menu_window(void);

/**
 * Draws the entire menu
 * @see draw_menu_iter(void)
 */
void draw_menu_full(void);

/**
 * Draws only the parts of the menu that can dynamically change
 * Suitable for calling in loops as it does less work than draw_menu_full
 * @see draw_menu_full(void)
 */
void draw_menu_iter(void);

/**
 * Finds the relative position of a color tile in the menu
 * @param index Index in the color list
 * @return Relative position of found tile; or VECTOR_INVALID if index is out of bounds
 */
Vector2i menu_tile_pos(unsigned index);

/**
 * Finds the relative position of the default color picker button
 * @return Relative position of found button
 */
Vector2i menu_cdef_pos(void);

/**
 * Cycle through preset logo sprites and about/help text
 */
void menu_cycle_logo(void);


/*----------------------------------------------------------------------------*
 *                              menu_controls.c                               *
 *----------------------------------------------------------------------------*/

/**
 * Deletes the old simulation and settings and sets them to the passed state
 * @param sim Simulation whose state to use
 * @return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED
 * @see reset_simulation(void)
 * @see clear_simulation(void)
 */
state_t set_simulation(Simulation *sim);

/**
 * Resets and remakes the active simulation using the current settings
 * @return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED
 * @see clear_simulation(void)
 * @see set_simulation(Simulation *)
 */
state_t reset_simulation(void);

/**
 * Clears the current settings and resets the active simulation
 * @return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED
 * @see reset_simulation(void)
 * @see set_simulation(Simulation *)
 */
state_t clear_simulation(void);

/**
 * Handles key commands passed to the menu window
 * @param key Key that was pressed
 * @param mouse Pointer to mouse event if one happened; NULL otherwise
 * @return STATE_GRID_CHANGED if grid changed | STATE_MENU_CHANGED if menu changed;
 *         STATE_NO_CHANGE otherwise
 * @see menu_mouse_command(MEVENT *)
 */
state_t menu_key_command(int key, MEVENT *mouse);

/**
 * Handles mouse commands passed to the menu window
 * @param mouse Pointer to mouse event if one happened; NULL otherwise
 * @return STATE_GRID_CHANGED if grid changed | STATE_MENU_CHANGED if menu changed
 *         | STATE_COLORS_CHANGED if color rules changed; STATE_NO_CHANGE otherwise
 * @see menu_key_command(int)
 */
state_t menu_mouse_command(MEVENT *mouse);


/*----------------------------------------------------------------------------*
 *                                  dialog.c                                  *
 *----------------------------------------------------------------------------*/

/**
 * Opens a temporary dialog window for picking colors
 * @param pos Dialog origin relative to menu
 * @param color_index Index of the color that is to be set (CIDX_NEWCOLOR to add a color,
 *        CIDX_DEFAULT to change the default)
 * @see close_dialog(void)
 */
void open_dialog(Vector2i pos, color_t color_index);

/**
 * Closes dialog and ends drawing
 * @see open_dialog(Vector2i, color_t)
 */
void close_dialog(void);

/**
 * Draws the dialog window
 */
void draw_dialog(void);

/**
 * Finds the relative position of a color tile in the dialog
 * @param color Color of tile
 * @return Relative position of found tile; or VECTOR_INVALID if color matches default
 *         or is invalid
 */
Vector2i dialog_tile_pos(color_t color);

/**
 * Handles mouse commands passed to the dialog window
 * @param mouse Pointer to mouse event if one happened; NULL otherwise
 * @return STATE_MENU_CHANGED if dialog/menu changed | STATE_COLORS_CHANGED if color
 *         rules changed; STATE_NO_CHANGE otherwise
 */
state_t dialog_mouse_command(MEVENT *mouse);

#endif  // __GRAPHICS_H__
