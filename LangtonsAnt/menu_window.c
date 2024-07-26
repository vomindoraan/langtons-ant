#include "graphics.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

WINDOW *menuw;
Settings stgs;
IOStatus load_status, save_status;

const Vector2i menu_pos         = { 0, GRID_WINDOW_SIZE };
const Vector2i menu_isize_u_pos = { MENU_ISIZE_Y+2,     MENU_RIGHT_COL+9 };
const Vector2i menu_isize_d_pos = { MENU_ISIZE_Y+5,     MENU_RIGHT_COL+9 };
const Vector2i menu_dir_u_pos   = { MENU_DIRECTION_Y+2, MENU_RIGHT_COL+10 };
const Vector2i menu_dir_r_pos   = { MENU_DIRECTION_Y+4, MENU_RIGHT_COL+14 };
const Vector2i menu_dir_d_pos   = { MENU_DIRECTION_Y+7, MENU_RIGHT_COL+10 };
const Vector2i menu_dir_l_pos   = { MENU_DIRECTION_Y+4, MENU_RIGHT_COL+7 };
const Vector2i menu_speed_u_pos = { MENU_SPEED_Y+2,     MENU_RIGHT_COL+9 };
const Vector2i menu_speed_d_pos = { MENU_SPEED_Y+21,    MENU_RIGHT_COL+9 };
const Vector2i menu_stepup_pos  = { MENU_SPEED_Y+20,    MENU_RIGHT_COL+1 };
const Vector2i menu_play_pos    = { MENU_CONTROLS_Y,    MENU_LEFT_COL+2 };
const Vector2i menu_pause_pos   = { MENU_CONTROLS_Y,    MENU_LEFT_COL+MENU_BUTTON_WIDTH+4 };
const Vector2i menu_stop_pos    = { MENU_CONTROLS_Y,    MENU_LEFT_COL+2*MENU_BUTTON_WIDTH+6 };
const Vector2i menu_load_pos    = { MENU_CONTROLS_Y-2*MENU_BUTTON_HEIGHT-4, MENU_RIGHT_COL+15-MENU_BUTTON_WIDTH };
const Vector2i menu_save_pos    = { MENU_CONTROLS_Y-MENU_BUTTON_HEIGHT-2,   MENU_RIGHT_COL+15-MENU_BUTTON_WIDTH };

static const char *logo_msg   = " 14-COLOR 2D TURING MACHINE SIMULATOR ";
static const char *rules_msg  = "COLOR RULES:";
static const char *isize_msg  = "INIT GRID SIZE:";
static const char *dir_msg    = "ANT DIRECTION:";
static const char *speed_msg  = "SIMULATION SPEED";
static const char *stepup_msg = "STEP+";
static const char *func_msg   = "STATE FUNCTION:";
static const char *size_msg   = "GRID SIZE:";
static const char *sparse_msg = "SPARSE";
static const char *steps_msg  = "STEPS:";

static const Vector2i logo_pos       = { MENU_LOGO_Y,       MENU_LEFT_COL+1 };
static const Vector2i logo_msg_pos   = { MENU_LOGO_Y+9,     MENU_LEFT_COL+2 };
static const Vector2i rules_pos      = { MENU_RULES_Y+5,    MENU_LEFT_COL+MENU_TILE_SIZE+MENU_TILE_HSEP+3 };
static const Vector2i rules_msg_pos  = { MENU_RULES_Y,      MENU_LEFT_COL+2 };
static const Vector2i isize_pos      = { MENU_ISIZE_Y+2,    MENU_RIGHT_COL+13 };
static const Vector2i isize_msg_pos  = { MENU_ISIZE_Y,      MENU_RIGHT_COL };
static const Vector2i dir_msg_pos    = { MENU_DIRECTION_Y,  MENU_RIGHT_COL };
static const Vector2i speed_pos      = { MENU_SPEED_Y+2,    MENU_RIGHT_COL+13 };
static const Vector2i speed_msg_pos  = { MENU_SPEED_Y,      MENU_RIGHT_COL };
static const Vector2i stepup_msg_pos = { MENU_SPEED_Y+18,   MENU_RIGHT_COL };
static const Vector2i func_pos       = { MENU_FUNCTION_Y+2, MENU_RIGHT_COL+4 };
static const Vector2i func_msg_pos   = { MENU_FUNCTION_Y,   MENU_RIGHT_COL };
static const Vector2i size_pos       = { MENU_STATUS_Y,     MENU_LEFT_COL+12 };
static const Vector2i size_msg_pos   = { MENU_STATUS_Y,     MENU_LEFT_COL+2 };
static const Vector2i sparse_msg_pos = { MENU_STATUS_Y+3,   MENU_LEFT_COL+2 };
static const Vector2i steps_pos      = { MENU_STATUS_Y+2,   MENU_LEFT_COL+9 };
static const Vector2i steps_msg_pos  = { MENU_STATUS_Y+6,   MENU_LEFT_COL+2 };

static const byte logo_sprite[] = {
	0x70, 0x00, 0x02, 0x00, 0x10, 0x20, 0x00, 0x02,
	0x00, 0x10, 0x20, 0xEE, 0x3B, 0x99, 0xC6, 0x21,
	0x29, 0x4A, 0x25, 0x24, 0x25, 0x29, 0x4A, 0x25,
	0x22, 0x7C, 0xE9, 0x39, 0x99, 0x26, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00
};
static const byte logo_highlight_sprite[] = {
	0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0xEE, 0x03, 0x80, 0x00, 0x01,
	0x29, 0x02, 0x00, 0x00, 0x01, 0x29, 0x02, 0x00,
	0x00, 0x00, 0xE9, 0x01, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const byte arrow_sprites[][1] = {
	{ 0x5C }, { 0xB8 }, { 0xE8 }, { 0x74 }
};
static const byte stepup_sprite[] = { 0x5D, 0x00 }; // > 0xCF, + 0x5D
static const byte digit_sprites[][2] = {
	{ 0xF6, 0xDE }, { 0x24, 0x92 }, { 0xE7, 0xCE }, { 0xE7, 0x9E }, { 0xB7, 0x92 },
	{ 0xF3, 0x9E }, { 0xF3, 0xDE }, { 0xE4, 0x92 }, { 0xF7, 0xDE }, { 0xF7, 0x9E }
};
static const byte inf_sprite[] = {
	0x00, 0x00, 0x07, 0x1C, 0x00, 0x00, 0x11, 0x44, 0x00, 0x00,
	0x21, 0x08, 0x00, 0x00, 0x45, 0x10, 0x00, 0x00, 0x71, 0xC0
};
static const byte button_sprites[][4] = {
	{ 0x43, 0x1C, 0xC4, 0x00 }, { 0x02, 0x94, 0xA0, 0x00 },
	{ 0x03, 0x9C, 0xE0, 0x00 }, { 0x47, 0x92, 0x17, 0x00 }
};

static size_t state_map[COLOR_COUNT] = { 0 };

void init_menu_window(void)
{
	menuw = newwin(MENU_WINDOW_HEIGHT, MENU_WINDOW_WIDTH, menu_pos.y, menu_pos.x);
	wbkgd(menuw, fg_pair);
	keypad(menuw, TRUE);
	nodelay(menuw, TRUE);
	assert(!IS_COLOR_BRIGHT(MENU_EDGE_COLOR)
	    && !IS_COLOR_BRIGHT(MENU_EDGE_COLOR_S)
	    && !IS_COLOR_BRIGHT(MENU_ACTIVE_COLOR)
	    && !IS_COLOR_BRIGHT(MENU_INACTIVE_COLOR));
}

void end_menu_window(void)
{
	delwin(menuw);
	menuw = NULL;
}

Vector2i get_menu_tile_pos(size_t index)
{
	Vector2i pos;
	size_t index_x, index_y;

	if (index < 0 || index >= MENU_TILE_COUNT) {
		return VECTOR_INVALID;
	}

	index_x = index / MENU_TILES_PER_COL;
	index_y = index % MENU_TILES_PER_COL;
	if (index_x % 2) {
		index_y = MENU_TILES_PER_COL - index_y - 1;
	}

	pos.y = rules_pos.y + index_y*(MENU_TILE_SIZE+MENU_TILE_VSEP);
	pos.x = rules_pos.x - index_x*(MENU_TILE_SIZE+MENU_TILE_HSEP);

	return pos;
}

Vector2i get_menu_cdef_pos(void)
{
	return (Vector2i) {
		.y = get_menu_tile_pos(min(stgs.colors->n, MENU_TILES_PER_COL)).y + MENU_TILE_SIZE + MENU_TILE_VSEP + 1,
		.x = rules_pos.x - MENU_TILE_SIZE - MENU_TILE_HSEP + 1
	};
}

static void draw_edge(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t h = MENU_WINDOW_WIDTH, v = MENU_WINDOW_HEIGHT;

	if (sim && is_grid_sparse(sim->grid)) {
		wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR_S));
		mvwaddstr(menuw, sparse_msg_pos.y, sparse_msg_pos.x, sparse_msg);
	} else {
		wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
		mvwhline(menuw, sparse_msg_pos.y, sparse_msg_pos.x, ' ', strlen(sparse_msg));
	}

	mvwhline(menuw, 0,   0,   FILL_CHAR, h);
	mvwvline(menuw, 0,   0,   FILL_CHAR, v);
	mvwhline(menuw, v-1, 0,   FILL_CHAR, h);
	mvwvline(menuw, 0,   h-1, FILL_CHAR, v);
}

static void draw_logo(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { logo_sprite, 40, 8 }, logo_pos, FALSE);
	wattron(menuw, A_REVERSE);
	mvwaddstr(menuw, logo_msg_pos.y, logo_msg_pos.x, logo_msg);
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR)); // TODO add copyright window
	draw_sprite(menuw, (SpriteInfo) { logo_highlight_sprite, 40, 8 }, logo_pos, FALSE);
}

static void draw_color_arrow(Vector2i pos1, Vector2i pos2)
{
	int ts = MENU_TILE_SIZE, o = MENU_TILE_SIZE/2, dy, dx;

	assert(pos1.y != pos2.y || pos1.x != pos2.x);
	wattrset(menuw, fg_pair);

	if (pos1.x == pos2.x) {
		dy = abs(pos1.y - pos2.y) - ts;
		mvwvline(menuw, min(pos1.y, pos2.y)+ts, pos1.x+o, ACS_VLINE, dy);
		if (pos1.y < pos2.y) {
			mvwaddch(menuw, pos2.y-1,  pos1.x+o, ACS_DARROW);
		} else {
			mvwaddch(menuw, pos2.y+ts, pos1.x+o, ACS_UARROW);
		}
	} else if (pos1.y == pos2.y) {
		dx = abs(pos1.x - pos2.x);
		dy = MENU_TILE_VSEP;
		if (pos1.x > pos2.x) {
			mvwvline(menuw, pos1.y+ts,    pos1.x+o, ACS_VLINE, dy);
			mvwhline(menuw, pos1.y+ts+dy, pos2.x+o, ACS_HLINE, dx);
			mvwvline(menuw, pos1.y+ts,    pos2.x+o, ACS_VLINE, dy);
			mvwaddch(menuw, pos1.y+ts+dy, pos1.x+o, ACS_LRCORNER);
			mvwaddch(menuw, pos1.y+ts+dy, pos2.x+o, ACS_LLCORNER);
			mvwaddch(menuw, pos1.y+ts,    pos2.x+o, ACS_UARROW);
		} else {
			mvwvline(menuw, pos1.y-dy-1,  pos1.x+o, ACS_VLINE, dy+1);
			mvwhline(menuw, pos1.y-dy-1,  pos1.x+o, ACS_HLINE, dx);
			mvwvline(menuw, pos1.y-dy-1,  pos2.x+o, ACS_VLINE, dy+1);
			mvwaddch(menuw, pos1.y-dy-1,  pos1.x+o, ACS_ULCORNER);
			mvwaddch(menuw, pos1.y-dy-1,  pos2.x+o, ACS_URCORNER);
			mvwaddch(menuw, pos1.y-1,     pos2.x+o, ACS_DARROW);
		}
	} else {
		dy = pos1.y - pos2.y;
		mvwvline(menuw, pos2.y, pos1.x+o, ACS_VLINE, dy);
		pos1.y = pos2.y;
		draw_color_arrow(pos1, pos2);
	}
}

static void draw_color_tile(Vector2i top_left, color_t c)
{
	chtype tile_pair = GET_PAIR_FOR(c);
	chtype frame_pair = (c != GET_COLOR_FOR(bg_pair)) ? tile_pair : ui_pair;
	bool is_def = c == stgs.colors->def;
	int y = top_left.y, x = top_left.x, s = MENU_TILE_SIZE;

	/* Draw tile */
	wattrset(menuw, is_def ? bg_pair : tile_pair);
	draw_square(menuw, top_left, s);

	/* Draw frame */
	wattrset(menuw, is_def ? frame_pair : fg_pair);
	mvwhline(menuw, y,     x,     FILL_CHAR, s);
	mvwvline(menuw, y,     x,     FILL_CHAR, s);
	mvwhline(menuw, y+s-1, x,     FILL_CHAR, s);
	mvwvline(menuw, y,     x+s-1, FILL_CHAR, s);

	/* Draw direction arrow */
	if (!is_def) {
		wattrset(menuw, tile_pair | A_REVERSE);
		mvwaddch(menuw, y+s/2, x+s/2, turn2arrow(stgs.colors->turn[c]));
	}
}

static void draw_color_list(void)
{
	color_t c;
	size_t i = 0;
	bool do_for = TRUE;
	Vector2i pos1, pos2, cdef_pos;

	pos1.y = rules_pos.y-MENU_TILE_VSEP-1, pos1.x = rules_pos.x-MENU_TILE_HSEP-MENU_TILE_SIZE;
	wattrset(menuw, bg_pair);
	draw_rect(menuw, pos1, MENU_TILES_WIDTH, MENU_TILES_HEIGHT);

	if (!stgs.colors) {
		return;
	}

	/* Draw color tiles */
	for (c = stgs.colors->first; do_for; c = stgs.colors->next[c]) {
		pos1 = pos2 = get_menu_tile_pos(i++);
		if (c == COLOR_NONE) {
			break;
		}
		if (i < MENU_TILE_COUNT) {
			pos2 = get_menu_tile_pos(i);
			draw_color_arrow(pos1, pos2);
		}
		draw_color_tile(pos1, c);
		state_map[c] = i;
		do_for = c != stgs.colors->last;
	}

	/* Draw placeholder tile */
	if (i < MENU_TILE_COUNT) {
		draw_color_tile(pos2, stgs.colors->def);
	}

	/* Draw arrow back to first tile */
	if (i >= MENU_TILES_PER_COL) {
		draw_color_arrow(pos2, get_menu_tile_pos(0));
	} else {
		pos1.y = pos2.y, pos1.x = pos2.x-MENU_TILE_SIZE-MENU_TILE_HSEP;
		draw_color_arrow(pos2, pos1);
		pos1.y += MENU_TILE_SIZE+1;
		draw_color_arrow(pos1, get_menu_tile_pos(0));
	}

	/* Draw default color picker message */
	cdef_pos = get_menu_cdef_pos();
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	mvwaddstr(menuw, cdef_pos.y, cdef_pos.x, dialog_cdef_msg);
}

static void draw_init_size(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_UP],   MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_isize_u_pos, FALSE);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_DOWN], MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_isize_d_pos, FALSE);
	wattrset(menuw, fg_pair);
	draw_sprite(menuw, (SpriteInfo) { digit_sprites[stgs.init_size], 3, 5 }, isize_pos, TRUE);
}

static void draw_dir_arrow(void)
{
	wattrset(menuw, fg_pair);
	mvwaddch(menuw, menu_dir_u_pos.y+3, menu_dir_u_pos.x+1, dir2arrow(stgs.linked_sim->ant->dir));
}

static void draw_direction(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_UP],    MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_dir_u_pos, FALSE);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_RIGHT], MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT },
	            menu_dir_r_pos, FALSE);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_DOWN],  MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_dir_d_pos, FALSE);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_LEFT],  MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT },
	            menu_dir_l_pos, FALSE);
	draw_dir_arrow();
}

static void draw_speed(void)
{
	chtype pair = GET_PAIR_FOR(MENU_ACTIVE_COLOR);
	int dy = menu_speed_d_pos.y - menu_speed_u_pos.y - 2;
	Vector2i slider_pos = { speed_pos.y + dy+1 - 2*stgs.speed, speed_pos.x }; // TODO better way for speed slider pos

	/* Draw scrollbar */
	wattrset(menuw, ui_pair);
	mvwvline(menuw, menu_speed_u_pos.y + 2, menu_speed_u_pos.x + 1, ACS_VLINE, dy);

	/* Draw slider */
	wattrset(menuw, fg_pair);
	mvwvline(menuw, slider_pos.y+1, slider_pos.x-3, FILL_CHAR, 3);

	/* Draw arrow buttons */
	wattrset(menuw, pair);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_UP], MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_speed_u_pos, FALSE);
	draw_sprite(menuw, (SpriteInfo) { arrow_sprites[DIR_DOWN], MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT },
	            menu_speed_d_pos, FALSE);

	/* Draw speed value */
	wattrset(menuw, bg_pair);
	draw_rect(menuw, speed_pos, MENU_DIGIT_WIDTH, dy + 4);
	wattrset(menuw, fg_pair);
	draw_sprite(menuw, (SpriteInfo) { digit_sprites[stgs.speed], 3, 5 }, slider_pos, TRUE);

	/* Draw Step+ sprite */
	wattrset(menuw, GET_PAIR_FOR(has_enough_colors(stgs.colors) ? MENU_ACTIVE_COLOR : MENU_INACTIVE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { stepup_sprite, 3, 3 }, menu_stepup_pos, FALSE);
}

static void draw_state_func(void)
{
	Simulation *sim = stgs.linked_sim;
	chtype pair = GET_PAIR_FOR(MENU_ACTIVE_COLOR);
	char str[8];
	color_t ant_color = GRID_ANT_COLOR(sim->grid, sim->ant);
	color_t next_color = sim->colors->next[ant_color]; // Uses sim->colors instead of stgs.colors

	sprintf(str, "f(q%-2hd, ", state_map[ant_color]);
	wattrset(menuw, pair);
	mvwaddstr(menuw, func_pos.y, func_pos.x, str);
	wattrset(menuw, GET_PAIR_FOR(ant_color));
	waddch(menuw, ACS_CKBOARD);
	wattrset(menuw, pair);
	waddstr(menuw, ") = ");

	sprintf(str, "(q%-2hd, ", state_map[next_color]);
	mvwaddstr(menuw, func_pos.y+1, func_pos.x+1, str);
	wattrset(menuw, GET_PAIR_FOR(next_color));
	waddch(menuw, ACS_CKBOARD);
	sprintf(str, ", %c) ", turn2arrow(sim->colors->turn[ant_color]));
	wattrset(menuw, pair);
	waddstr(menuw, str);
}

static void draw_control_buttons(void)
{
	Vector2i o = { (MENU_BUTTON_HEIGHT-5)/2, (MENU_BUTTON_WIDTH-5)/2 };
	Vector2i pos1 = { menu_play_pos.y  + o.y, menu_play_pos.x  + o.x };
	Vector2i pos2 = { menu_pause_pos.y + o.y, menu_pause_pos.x + o.x };
	Vector2i pos3 = { menu_stop_pos.y  + o.y, menu_stop_pos.x  + o.x };

	wattrset(menuw, ui_pair);
	draw_rect(menuw, menu_play_pos,  MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_pause_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_stop_pos,  MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

	wattrset(menuw, GET_PAIR_FOR(has_enough_colors(stgs.colors) ? MENU_PLAY_COLOR : MENU_INACTIVE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { button_sprites[0], 5, 5 }, pos1, FALSE);

	wattrset(menuw, GET_PAIR_FOR(is_simulation_running(stgs.linked_sim) ? MENU_PAUSE_COLOR : MENU_INACTIVE_COLOR));
	draw_sprite(menuw, (SpriteInfo) { button_sprites[1], 5, 5 }, pos2, FALSE);

	if (has_simulation_started(stgs.linked_sim)) {
		wattrset(menuw, GET_PAIR_FOR(MENU_STOP_COLOR));
		draw_sprite(menuw, (SpriteInfo) { button_sprites[2], 5, 5 }, pos3, FALSE);
	} else {
		wattrset(menuw, GET_PAIR_FOR(!is_colors_empty(stgs.colors) ? MENU_CLEAR_COLOR : MENU_INACTIVE_COLOR));
		draw_sprite(menuw, (SpriteInfo) { button_sprites[3], 5, 5 }, pos3, FALSE);
	}
}

static void draw_io_buttons(void)
{
	Vector2i inner1 = { menu_load_pos.y+1, menu_load_pos.x+1 };
	Vector2i inner2 = { menu_save_pos.y+1, menu_save_pos.x+1 };

	wattrset(menuw, ui_pair);
	draw_rect(menuw, menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	wattrset(menuw, bg_pair);
	draw_rect(menuw, inner1, MENU_BUTTON_WIDTH-2, MENU_BUTTON_HEIGHT-2);
	draw_rect(menuw, inner2, MENU_BUTTON_WIDTH-2, MENU_BUTTON_HEIGHT-2);

	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, inner1.y+1, inner1.x, " LOAD    ");
	mvwaddstr(menuw, inner1.y+2, inner1.x, "  FROM   ");
	mvwaddstr(menuw, inner1.y+3, inner1.x, "    FILE ");
	mvwaddstr(menuw, inner2.y+1, inner2.x, " SAVE    ");
	mvwaddstr(menuw, inner2.y+2, inner2.x, "   TO    ");
	mvwaddstr(menuw, inner2.y+3, inner2.x, "    FILE ");

	/* Draw status indicators */
	if (load_status != STATUS_NONE) {
		wattrset(menuw, GET_PAIR_FOR((load_status == STATUS_SUCCESS) ? COLOR_LIME : COLOR_RED));
		mvwvline(menuw, menu_load_pos.y, menu_load_pos.x+MENU_BUTTON_WIDTH, FILL_CHAR, MENU_BUTTON_HEIGHT);
	}
	if (save_status != STATUS_NONE) {
		wattrset(menuw, GET_PAIR_FOR((save_status == STATUS_SUCCESS) ? COLOR_LIME : COLOR_RED));
		mvwvline(menuw, menu_save_pos.y, menu_save_pos.x+MENU_BUTTON_WIDTH, FILL_CHAR, MENU_BUTTON_HEIGHT);
	}
}

static void draw_grid_size(void)
{
	Simulation *sim = stgs.linked_sim;
	size_t size = sim ? sim->grid->size : 0;
	char str[29];
	sprintf(str, "%28d", size);
	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, size_pos.y, size_pos.x, str);
}

static void draw_steps(void)
{
	static bool do_draw;
	Simulation *sim = stgs.linked_sim;
	size_t steps = sim ? sim->steps : 0;
	int len = (int)log10(steps) + 1;
	Vector2i tl = steps_pos;
	char digits[9], *d;

	if (steps <= 1) {
		do_draw = TRUE;
	} else if (!do_draw) {
		return;
	}

	wattrset(menuw, fg_pair);
	if (len > 8) {
		draw_sprite(menuw, (SpriteInfo) { inf_sprite, 31, 5 }, tl, TRUE);
		do_draw = FALSE;
		return;
	}

	sprintf(digits, "%8d", steps);
	for (d = digits; d < digits+8; d++) {
		if (*d != ' ') {
			int digit = *d - '0';
			wattrset(menuw, fg_pair);
			draw_sprite(menuw, (SpriteInfo) { digit_sprites[digit], 3, 5 }, tl, TRUE);
		} else {
			wattrset(menuw, bg_pair);
			draw_rect(menuw, tl, 3, 5);
		}
		tl.x += 4;
	}
}

static void draw_labels(void)
{
	wattrset(menuw, GET_PAIR_FOR(MENU_EDGE_COLOR));
	mvwaddstr(menuw, rules_msg_pos.y,  rules_msg_pos.x,  rules_msg);
	mvwaddstr(menuw, isize_msg_pos.y,  isize_msg_pos.x,  isize_msg);
	mvwaddstr(menuw, dir_msg_pos.y,    dir_msg_pos.x,    dir_msg);
	mvwaddstr(menuw, speed_msg_pos.y,  speed_msg_pos.x,  speed_msg);
	mvwaddstr(menuw, stepup_msg_pos.y, stepup_msg_pos.x, stepup_msg);
	mvwaddstr(menuw, func_msg_pos.y,   func_msg_pos.x,   func_msg);
	mvwaddstr(menuw, size_msg_pos.y,   size_msg_pos.x,   size_msg);
	mvwaddstr(menuw, steps_msg_pos.y,  steps_msg_pos.x,  steps_msg);
}

void draw_menu_full(void)
{
	draw_edge();
	draw_logo();
	draw_color_list();
	draw_init_size();
	draw_direction();
	draw_speed();
	draw_state_func();
	draw_control_buttons();
	draw_io_buttons();
	draw_grid_size();
	draw_steps();
	draw_labels();
	wnoutrefresh(menuw);

	if (dialogw) {
		draw_dialog();
	}
}

void draw_menu_iter(void)
{
	Simulation *sim = stgs.linked_sim;
	static bool sparse = FALSE;
#if LOOP_OPT_ENABLE
	static size_t prev_steps = 0;
	size_t threshold = (stgs.speed == LOOP_MAX_SPEED) ? LOOP_MAX_OPT
	                 : LOOP_DEF_OPT * (stgs.speed - LOOP_OPT_SPEED + 1);
	bool do_draw = sim->steps-prev_steps >= threshold;
#endif

	if (!sparse && is_grid_sparse(sim->grid)) {
		sparse = TRUE;
#if LOOP_OPT_ENABLE
		do_draw = TRUE;
#endif
		draw_edge();
	}
#if LOOP_OPT_ENABLE
	if (do_draw) {
#endif
		draw_dir_arrow();
		draw_state_func();
		draw_steps();
		wnoutrefresh(menuw);

		if (dialogw) {
			draw_dialog();
		}
#if LOOP_OPT_ENABLE
		prev_steps = sim->steps;
	}
#endif
}
