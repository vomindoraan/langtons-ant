/**
 * @file graphics.h
 * Graphics members and function declarations
 * @author IntelligAnt
 */
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "logic.h"

#include "include/curses.h"

/** @name Preferred console font settings */
///@{
#define CONSOLE_FONT_FACE L"Px437 IBM EGA 8x8"
#define CONSOLE_FONT_SIZE 8
///@}


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
#define COLOR_BLACK   0
#define COLOR_SILVER  7

#define COLOR_GRAY    8
#define COLOR_WHITE   15

#ifdef PDC_RGB        /* RGB */
#define COLOR_MAROON  1
#define COLOR_GREEN   2
#define COLOR_NAVY    4

#define COLOR_RED     9
#define COLOR_LIME    10
#define COLOR_BLUE    12
#else                 /* BGR */
#define COLOR_NAVY    1
#define COLOR_GREEN   2
#define COLOR_MAROON  4

#define COLOR_BLUE    9
#define COLOR_LIME    10
#define COLOR_RED     12
#endif

#define COLOR_TEAL    (COLOR_NAVY | COLOR_GREEN)
#define COLOR_PURPLE  (COLOR_MAROON | COLOR_NAVY)
#define COLOR_OLIVE   (COLOR_MAROON | COLOR_GREEN)

#define COLOR_AQUA    (COLOR_BLUE | COLOR_LIME)
#define COLOR_FUCHSIA (COLOR_RED | COLOR_BLUE)
#define COLOR_YELLOW  (COLOR_RED | COLOR_LIME)

#ifndef COLOR_COUNT
#define COLOR_COUNT   16
#endif
#ifndef COLOR_NONE
#define COLOR_NONE    -1
#endif
///@}

/** @name Utility color macros */
///@{
#define GET_PAIR_FOR(c)             (COLOR_PAIR((c) + 1))
#define GET_COLOR_FOR(p)            (PAIR_NUMBER(p) - 1)
#define AVAILABLE_COLOR(def, c, bk) (((def) == (c)) ? (bk) : (c))
#define AVAILABLE_PAIR(def, c, bk)  GET_PAIR_FOR(AVAILABLE_COLOR(def, c, bk))
#define IS_COLOR_BRIGHT(c)          (((c)&1 + !!((c)&2) + !!((c)&4) + !!((c)&8)) > 2)
///@}


/*----------------------- Grid window macros and types -----------------------*/

/** @name Grid window attributes */
///@{
#define GRID_WINDOW_SIZE    109
#define GRID_VIEW_SIZE      (GRID_WINDOW_SIZE - 1)
#define LINE_WIDTH_SMALL    2
#define LINE_WIDTH_MEDIUM   1
#define LINE_WIDTH_LARGE    0
#define SCROLL_STEP_SMALL   (GRID_VIEW_SIZE / 18)
#define SCROLL_STEP_MEDIUM  (GRID_VIEW_SIZE / 4)
#define SCROLL_STEP_LARGE   GRID_VIEW_SIZE
///@}

/** @name Generic size calculations for grid functions */
///@{
#define CELL_SIZE(gs, lw)      ((GRID_WINDOW_SIZE-(lw))/(gs) - (lw))
#define TOTAL_SIZE(gs, lw, cs) (((gs)+1)*(lw) + (gs)*(cs))
#define OFFSET_SIZE(t)         ((GRID_WINDOW_SIZE-(t)) / 2)
///@}

/** @name Scroll utility macros */
///@{
#define ORIGIN_COORD(gs, vs, sc)     ((gs)/2-(vs)/2+(sc))
#define ORIGIN_POS(gs, vs, scy, scx) (Vector2i) { ORIGIN_COORD(gs, vs, scy), ORIGIN_COORD(gs, vs, scx) }
///@}

/** Structure for managing scroll data */
typedef struct scroll_info {
	bool enabled;         /**< Scrolling is enabled */                    /**@{*/
	int y, x;             /**< Current view position relative to (0,0) */ /**@}*/ /**@{*/
	int hcenter, vcenter; /**< Scrollbar slider positions */              /**@}*/
	double scale;         /**< Scaling multiplier */
} ScrollInfo;


/*----------------------- Menu window macros and types -----------------------*/

/** @name Menu window attributes */
///@{
#define MENU_WINDOW_WIDTH   42
#define MENU_WINDOW_HEIGHT  GRID_WINDOW_SIZE
#define MENU_LOGO_HEIGHT    16
#define MENU_CONTROLS_POS   89
#define MENU_STATUS_POS     (MENU_WINDOW_HEIGHT-10)
#define MENU_RIGHT_COLUMN   (MENU_WINDOW_WIDTH-18)
#define MENU_DIRECTION_POS  (MENU_LOGO_HEIGHT+10)
#define MENU_SPEED_POS      (MENU_DIRECTION_POS+12)
#define MENU_FUNCTION_POS   (MENU_SPEED_POS+26)
#define MENU_EDGE_COLOR     COLOR_NAVY
#define MENU_EDGE_COLOR_S   COLOR_MAROON
#define MENU_ACTIVE_COLOR   COLOR_BLUE
#define MENU_INACTIVE_COLOR COLOR_GRAY
///@}

/** @name Menu buttons attributes */
///@{
#define MENU_BUTTON_WIDTH   11
#define MENU_BUTTON_HEIGHT  7
#define MENU_PLAY_COLOR     COLOR_GREEN
#define MENU_PAUSE_COLOR    COLOR_YELLOW
#define MENU_STOP_COLOR     COLOR_RED
#define MENU_CLEAR_COLOR    COLOR_TEAL
///@}

/** @name Menu sprite dimensions */
///@{
#define MENU_UDARROW_WIDTH  3
#define MENU_UDARROW_HEIGHT 2
#define MENU_RLARROW_WIDTH  MENU_UDARROW_HEIGHT
#define MENU_RLARROW_HEIGHT MENU_UDARROW_WIDTH
#define MENU_DIGIT_WIDTH    3
#define MENU_DIGIT_HEIGHT   5
#define MENU_STEPUP_SIZE    3
///@}

/** @name Menu color tiles attributes */
///@{
#define MENU_TILE_SIZE      7
#define MENU_TILE_HSEP      3
#define MENU_TILE_VSEP      2
#define MENU_TILE_COLUMNS   2
#define MENU_TILES_PER_COL  7
#define MENU_TILE_COUNT     (MENU_TILE_COLUMNS * MENU_TILES_PER_COL)
#define MENU_TILES_WIDTH    (MENU_TILE_COLUMNS*MENU_TILE_SIZE + (MENU_TILE_COLUMNS-1)*MENU_TILE_HSEP)
#define MENU_TILES_HEIGHT   (MENU_TILES_PER_COL*MENU_TILE_SIZE + (MENU_TILES_PER_COL+1)*MENU_TILE_VSEP + 3)
///@}

/** Structure containing all relevant menu settings */
typedef struct settings {
	Colors *colors;         /**< Currently active color rules */
	size_t init_size;       /**< Initial grid size */
	byte speed;             /**< Speed multiplier */
	Simulation *linked_sim; /**< Currently active simulation */
} Settings;

/** Status indicator type for IO operations in the menu */
typedef enum { STATUS_NONE, STATUS_SUCCESS, STATUS_FAILURE } IOStatus;


/*---------------------- Dialog window macros and types ----------------------*/

/** @name Dialog window attributes */
///@{
#define DIALOG_TILE_SIZE     3
#define DIALOG_TILE_ROWS     3
#define DIALOG_TILE_COLS     5
#define DIALOG_BUTTON_WIDTH  7
#define DIALOG_BUTTON_HEIGHT 3
#define DIALOG_BUTTON_COLOR  COLOR_WHITE
#define DIALOG_DELETE_WIDTH  5
#define DIALOG_DELETE_HEIGHT DIALOG_BUTTON_HEIGHT
#define DIALOG_DELETE_COLOR  COLOR_RED
#define DIALOG_WINDOW_WIDTH  (DIALOG_TILE_COLS*DIALOG_TILE_SIZE + 2)
#define DIALOG_WINDOW_HEIGHT (DIALOG_TILE_ROWS*DIALOG_TILE_SIZE + DIALOG_BUTTON_HEIGHT*2 + 4)
///@}

///@{
/** Designates which colors are to be set in the dialog */
#define CIDX_NEWCOLOR -1
#define CIDX_DEFAULT  -2
///@}


/*--------------------- Input handling macros and types ----------------------*/

/** Escape key literal for input handling */
#define KEY_ESC 0x1B

/** @name Window state change flags */
///@{
#define INPUT_NO_CHANGE    0
#define INPUT_GRID_CHANGED 1
#define INPUT_MENU_CHANGED 2
///@}

/** Window state change as a result of input events (bitwise OR of INPUT_* fields) */
typedef byte input_t;


/*------------------------- Loop performance macros --------------------------*/

/** @name Performance settings */
///@{
#define LOOP_DEF_SPEED 2 /**< Default speed multiplier */
#define LOOP_MIN_SPEED 1 /**< Minimum allowed speed multiplier */
#define LOOP_MAX_SPEED 9 /**< Maximum allowed speed multiplier */
#define LOOP_DELAY     (1 << (LOOP_MAX_SPEED-1)) /**< Base delay in ms */
#define LOOP_OPT_STEPS 1 /**< Should optimize drawing of steps in the menu by skipping? */
#define LOOP_OPT_SPEED 3 /**< Threshold speed at which to begin skipping */
///@}


/*------------------------------- Sprite type --------------------------------*/

/** Structure containing sprite data and size */
typedef struct sprite_info {
	const byte *data;     /**< Byte array */  /**@{*/
	size_t width, height; /**< Sprite size */ /**@}*/
} SpriteInfo;


/*------------------------ Global variables/constants ------------------------*/

/** @name Globals */
///@{
extern chtype         fg_pair, bg_pair, ui_pair;

extern WINDOW         *gridw;
extern ScrollInfo     gridscrl;
extern const Vector2i grid_pos;

extern WINDOW         *menuw;
extern Settings       stgs;
extern IOStatus       load_status, save_status;
extern const Vector2i menu_pos;
extern const Vector2i menu_isize_u_pos, menu_isize_d_pos;
extern const Vector2i menu_dir_u_pos, menu_dir_r_pos, menu_dir_d_pos, menu_dir_l_pos;
extern const Vector2i menu_speed_u_pos, menu_speed_d_pos, menu_stepup_pos;
extern const Vector2i menu_play_pos, menu_pause_pos, menu_stop_pos;
extern const Vector2i menu_load_pos, menu_save_pos;

extern WINDOW         *dialogw;
extern Vector2i       dialog_pos;
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
bool area_contains(Vector2i top_left, size_t width, size_t height, Vector2i v);

/**
 * Utility function for drawing square boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param size Box size
 * @see draw_rect(WINDOW *, Vector2i, size_t, size_t)
 */
void draw_square(WINDOW *w, Vector2i top_left, size_t size);

/**
 * Utility function for drawing rectangular boxes
 * @param w Window to draw to
 * @param top_left Box origin
 * @param width Box width
 * @param height Box height
 * @see draw_square(WINDOW *, Vector2i, size_t)
 */
void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height);

/**
 * Utility function for drawing thin rectangular borders
 * @param w Window to draw to
 * @param top_left Box origin
 * @param width Box width
 * @param height Box height
 */
void draw_border(WINDOW *w, Vector2i top_left, size_t width, size_t height);

/**
 * Utility function for drawing monochrome sprites
 * @param w Window to draw to
 * @param sprite Sprite to be drawn and its size as SpriteInfo
 * @param top_left Sprite origin
 * @param overwrite Should existing content be overwritten?
 */
void draw_sprite(WINDOW *w, SpriteInfo sprite_info, Vector2i top_left, bool overwrite);

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
 *                               ant_sprites.c                                *
 *----------------------------------------------------------------------------*/

/**
 * Finds a suitable sprite for the given cell size and ant direction
 * @param size Cell size
 * @param dir Current ant direction
 * @return Ant sprite with requested size and direction, if one exists; NULL otherwise
 */
SpriteInfo get_ant_sprite(size_t size, Direction dir);


/*----------------------------------------------------------------------------*
 *                                game_loop.c                                 *
 *----------------------------------------------------------------------------*/

/**
 * Main draw/update loop for the current simulation
 * @see stop_game_loop(void)
 * @see simulation_step(Simulation *)
 */
void game_loop(void);

/**
 * Stops the main draw/update loop
 * @see game_loop(void)
 */
void stop_game_loop(void);

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
 * @param old_pos Position of cell that has changed and should be drawn
 * @see draw_grid_full(Grid *)
 */
void draw_grid_iter(Grid *grid, Ant *ant, Vector2i old_pos);

/**
 * Scrolls the grid relative to the current gridscrl position
 * @param grid Grid from which to draw
 * @param dy Relative y offset
 * @param dx Relative x offset
 * @see set_scroll(Grid *, int, int)
 * @see reset_scroll(void)
 */
void scroll_grid(Grid *grid, int dy, int dx);

/**
 * Sets the absolute scroll of gridscrl
 * @param y Absolute y offset
 * @param x Absolute x offset
 * @see scroll_grid(Grid, int, int)
 * @see reset_scroll(void)
 */
void set_scroll(Grid *grid, int y, int x);

/**
 * Resets gridscrl to its initial state
 * @see scroll_grid(Grid *, int, int)
 * @see set_scroll(Grid *, int, int)
 */
void reset_scroll(void);


/*----------------------------------------------------------------------------*
 *                              grid_controls.c                               *
 *----------------------------------------------------------------------------*/

/**
 * Handles key command passed to the grid window
 * @param grid Grid to be acted upon
 * @param ant Ant to be acted upon
 * @param key Key passed to the grid
 * @return INPUT_GRID_CHANGED if grid changed; INPUT_NO_CHANGE otherwise
 * @see grid_mouse_command(Grid *)
 */
input_t grid_key_command(Grid *grid, Ant *ant, int key);

/**
 * Handles mouse command passed to the grid window
 * @param grid Grid to be acted upon
 * @return INPUT_GRID_CHANGED if grid changed; INPUT_NO_CHANGE otherwise
 * @see grid_key_command(Grid *, Ant *, int)
 */
input_t grid_mouse_command(Grid *grid);


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
Vector2i get_menu_tile_pos(size_t index);

/**
 * Finds the relative position of the default color picker button
 * @return Relative position of found button
 */
Vector2i get_menu_cdef_pos(void);


/*----------------------------------------------------------------------------*
 *                              menu_controls.c                               *
 *----------------------------------------------------------------------------*/

/**
 * Deletes the old simulation and settings and sets them to the passed state
 * @param sim Simulation whose state to use
 * @return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED
 * @see reset_simulation(void)
 * @see clear_simulation(void)
 */
input_t set_simulation(Simulation *sim);

/**
 * Resets and remakes the active simulation using the current settings
 * @return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED
 * @see clear_simulation(void)
 * @see set_simulation(Simulation *)
 */
input_t reset_simulation(void);

/**
 * Clears the current settings and resets the active simulation
 * @return INPUT_MENU_CHANGED | INPUT_GRID_CHANGED
 * @see reset_simulation(void)
 * @see set_simulation(Simulation *)
 */
input_t clear_simulation(void);

/**
 * Handles key Command passed to the menu
 * @param key Key passed to the grid
 * @return INPUT_MENU_CHANGED if menu changed | INPUT_GRID_CHANGED if grid changed;
 *         INPUT_NO_CHANGE otherwise
 * @see menu_mouse_command(void)
 */
input_t menu_key_command(int key);

/**
 * Handles mouse command passed to the menu
 * @return INPUT_MENU_CHANGED if menu changed | INPUT_GRID_CHANGED if grid changed;
 *         INPUT_NO_CHANGE otherwise
 * @see menu_key_command(int)
 */
input_t menu_mouse_command(void);


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
 * @return Relative position of found tile
 */
Vector2i get_dialog_tile_pos(color_t color);

/**
 * Handles mouse command passed to the dialog
 * @param event Mouse event
 */
input_t dialog_mouse_command(MEVENT event);

#endif
