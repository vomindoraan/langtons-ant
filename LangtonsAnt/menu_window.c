#include "graphics.h"
#include "version.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

WINDOW        *menuw;
Settings       stgs;
IOStatus       load_status, save_status;
PendingAction  pending_action;

const Vector2i  menu_pos              = { 0,                  GRID_WINDOW_SIZE };
const Vector2i  menu_logo_pos         = { MENU_LOGO_Y,        MENU_LEFT_COL_X };
const Vector2i  menu_isize_u_pos      = { MENU_INIT_SIZE_Y+2, MENU_RIGHT_COL_X+9 };
const Vector2i  menu_isize_d_pos      = { MENU_INIT_SIZE_Y+5, MENU_RIGHT_COL_X+9 };
const Vector2i  menu_dir_u_pos        = { MENU_DIRECTION_Y+2, MENU_RIGHT_COL_X+9 };
const Vector2i  menu_dir_r_pos        = { MENU_DIRECTION_Y+4, MENU_RIGHT_COL_X+13 };
const Vector2i  menu_dir_d_pos        = { MENU_DIRECTION_Y+7, MENU_RIGHT_COL_X+9 };
const Vector2i  menu_dir_l_pos        = { MENU_DIRECTION_Y+4, MENU_RIGHT_COL_X+6 };
const Vector2i  menu_stepup_pos       = { MENU_STEPUP_Y+2,    MENU_RIGHT_COL_X+9 };
const Vector2i  menu_speed_u_pos      = { MENU_SPEED_Y+2,     MENU_RIGHT_COL_X+9 };
const Vector2i  menu_speed_d_pos      = { MENU_SPEED_Y+MENU_SPEED_HEIGHT+4,   MENU_RIGHT_COL_X+9 };
const Vector2i  menu_play_pos         = { MENU_CONTROLS_Y,    MENU_PLAY_X };
const Vector2i  menu_stop_pos         = { MENU_CONTROLS_Y,    MENU_STOP_X };
const Vector2i  menu_load_pos         = { MENU_CONTROLS_Y,    MENU_LOAD_X };
#if SAVE_ENABLE
const Vector2i  menu_save_pos         = { MENU_CONTROLS_Y-MENU_BUTTON_PHEIGHT, MENU_SAVE_X };
#endif

static const Vector2i  logo_msg_pos   = { MENU_LOGO_Y+9,       MENU_LEFT_COL_X };
static const Vector2i  rules_pos      = { MENU_RULES_Y+5,      MENU_LEFT_COL_X+MENU_TILE_PWIDTH+1 };
static const Vector2i  rules_msg_pos  = { MENU_RULES_Y,        MENU_LEFT_COL_X };
static const Vector2i  isize_pos      = { MENU_INIT_SIZE_Y+2,  MENU_RIGHT_COL_X+13 };
static const Vector2i  isize_msg_pos  = { MENU_INIT_SIZE_Y,    MENU_RIGHT_COL_X };
static const Vector2i  dir_msg_pos    = { MENU_DIRECTION_Y,    MENU_RIGHT_COL_X };
static const Vector2i  stepup_msg_pos = { MENU_STEPUP_Y,       MENU_RIGHT_COL_X };
static const Vector2i  speed_pos      = { MENU_SPEED_Y+2,      MENU_RIGHT_COL_X+13 };
static const Vector2i  speed_msg_pos  = { MENU_SPEED_Y,        MENU_RIGHT_COL_X };
static const Vector2i  func_pos       = { MENU_STATE_FUNC_Y+2, MENU_RIGHT_COL_X+3 };
static const Vector2i  func_msg_pos   = { MENU_STATE_FUNC_Y,   MENU_RIGHT_COL_X };
static const Vector2i  sparse_msg_pos = { MENU_STATE_FUNC_Y+7, MENU_RIGHT_COL_X };
static const Vector2i  size_pos       = { MENU_STATUS_Y,       MENU_RIGHT_COL_X };
static const Vector2i  size_msg_pos   = { MENU_STATUS_Y,       MENU_LEFT_COL_X };
static const Vector2i  steps_pos      = { MENU_STATUS_Y+2,     MENU_LEFT_COL_X+7 };
static const Vector2i  steps_msg_pos  = { MENU_STATUS_Y+6,     MENU_LEFT_COL_X };

static const char *logo_msg           = " 14-COLOR 2D TURING MACHINE SIMULATOR ";
static const char *rules_msg          = "COLOR RULES";
static const char *isize_msg          = "INIT GRID SIZE";
static const char *dir_msg            = "ANT DIRECTION";
static const char *speed_msg          = "SIMULATION SPEED";
static const char *stepup_msg         = "STEP BY STEP";
static const char *func_msg           = "STATE FUNCTION";
static const char *sparse_msg         = "[SPARSE MATRIX]";
static const char *size_msg           = "GRID SIZE:";
static const char *steps_msg          = "STEPS:";

static const char *load_label[] = {
	" LOAD  ",
	"",
	"EXAMPLE",
};
#if SAVE_ENABLE
static const char *save_label[] = {
	" SAVE  ",
	"",
	"TO FILE",
};
#endif

typedef const char  logo_str_t[MENU_LOGO_WIDTH+1];

static logo_str_t logo_about_text[MENU_LOGO_HEIGHT] = {
	APP_NAME " v" APP_VERSION " (" APP_CURSES ")",
	"",
	" " APP_COPYRIGHT1,
	" " APP_COPYRIGHT2,
	" " APP_COPYRIGHT3,
	"",
	" " APP_URL_REPO,
	" " APP_URL_ABOUT,
	"",
	"            (click again for controls)",
};
static logo_str_t logo_help_text[MENU_LOGO_HEIGHT] = {
	"Controls:",
	"",
	" Use mouse to set & update color rules",
	"",
	"      play/pause   ^<v>  scroll view",
	" R    stop/reset   F M1  jump to ant",
	" Q +  faster       C M2  jump center",
	" Z -  slower       [ ]   init size",
	" E .  step         WASD  ant direction",
	" F1   load        "
#if SAVE_ENABLE
	" F2    save",
#endif
};

#define LOGO_INIT_SPRITES(s, c)  { .sprites = s, .hl_color = c, TRUE }
#define LOGO_INIT_TEXT(t, c)     { .text = t,    .hl_color = c, FALSE }

typedef struct logo {
	union {
		const LogoSprites *sprites;
		logo_str_t        *text;
	};
	color_t  hl_color;
	bool     has_sprites;
} Logo;

static const Logo logos[] = {
	LOGO_INIT_SPRITES(&logo1_sprites, MENU_INACTIVE_COLOR),
	LOGO_INIT_TEXT(logo_about_text,   MENU_INACTIVE_COLOR),
	LOGO_INIT_TEXT(logo_help_text,    MENU_INACTIVE_COLOR),
	LOGO_INIT_SPRITES(&logo2_sprites, MENU_ACTIVE_COLOR),
};
static unsigned logo_index;

static unsigned state_map[COLOR_COUNT];

void init_menu_window(void)
{
	menuw = newwin(MENU_WINDOW_HEIGHT, MENU_WINDOW_WIDTH, menu_pos.y, menu_pos.x);
	wbkgd(menuw, ui_pair);
	keypad(menuw, TRUE);
	nodelay(menuw, TRUE);
	assert(!IS_COLOR_BRIGHT(MENU_BORDER_COLOR)
	    && !IS_COLOR_BRIGHT(MENU_BORDER_COLOR_S)
	    && !IS_COLOR_BRIGHT(MENU_ACTIVE_COLOR)
	    && !IS_COLOR_BRIGHT(MENU_INACTIVE_COLOR));
}

void end_menu_window(void)
{
	delwin(menuw);
	menuw = NULL;
}

Vector2i menu_tile_pos(unsigned index)
{
	Vector2i pos;
	unsigned index_x, index_y;

	if (index >= MENU_TILES_COUNT) {
		return VECTOR_INVALID;
	}

	index_x = index / MENU_TILES_PER_COL;
	index_y = index % MENU_TILES_PER_COL;
	if (index_x % 2) {
		index_y = MENU_TILES_PER_COL - index_y - 1;
	}

	pos.y = rules_pos.y + index_y*MENU_TILE_PHEIGHT;
	pos.x = rules_pos.x - index_x*MENU_TILE_PWIDTH;
	return pos;
}

Vector2i menu_cdef_pos(void)
{
	Vector2i tile_pos = menu_tile_pos(MIN(stgs.colors->n, MENU_TILES_PER_COL));
	return (Vector2i) {
		.y = tile_pos.y + MENU_TILE_PHEIGHT + 1,
		.x = rules_pos.x - MENU_TILE_PWIDTH + 1,
	};
}

void menu_cycle_logo(void)
{
	logo_index = (logo_index+1) % LEN(logos);
}

static void draw_logo(void)
{
	const Logo *logo = &logos[logo_index];
	unsigned i;

	wattrset(menuw, bg_pair);
	draw_rect(menuw, menu_logo_pos, MENU_LOGO_WIDTH, MENU_LOGO_HEIGHT);

	if (logo->has_sprites) {
		wattrset(menuw, PAIR_FOR(MENU_BORDER_COLOR));
		draw_sprite(menuw, SI_LOGO(logo->sprites->base), menu_logo_pos, FALSE);

		wattron(menuw, A_REVERSE);
		mvwaddstr(menuw, logo_msg_pos.y, logo_msg_pos.x, logo_msg);

		wattrset(menuw, PAIR_FOR(logo->hl_color));
		draw_sprite(menuw, SI_LOGO(logo->sprites->hl), menu_logo_pos, FALSE);
	} else {
		wattrset(menuw, PAIR_FOR(MENU_BORDER_COLOR));
		for (i = 0; i < MENU_LOGO_HEIGHT; i++) {
			mvwaddstr(menuw, menu_logo_pos.y+i, menu_logo_pos.x, logo->text[i]);

			wattrset(menuw, PAIR_FOR(logo->hl_color));
		}
	}
}

static void draw_border(void)
{
	Simulation *sim = stgs.simulation;
	unsigned h = MENU_WINDOW_WIDTH, v = MENU_WINDOW_HEIGHT;

	if (sim && is_grid_sparse(sim->grid)) {
		wattrset(menuw, PAIR_FOR(MENU_BORDER_COLOR_S));
		mvwaddstr(menuw, sparse_msg_pos.y, sparse_msg_pos.x, sparse_msg);
	} else {
		wattrset(menuw, PAIR_FOR(MENU_BORDER_COLOR));
		mvwhline(menuw, sparse_msg_pos.y, sparse_msg_pos.x, CHAR_EMPTY, (int)strlen(sparse_msg));
	}

	mvwhline(menuw, 0,   0,   CHAR_FULL, h);
	mvwvline(menuw, 0,   0,   CHAR_FULL, v);
	mvwhline(menuw, v-1, 0,   CHAR_FULL, h);
	mvwvline(menuw, 0,   h-1, CHAR_FULL, v);
}

static void draw_color_tile(Vector2i top_left, color_t c)
{
	chtype tile_pair = PAIR_FOR(c);
	chtype frame_pair = (c != COLOR_FOR(bg_pair)) ? tile_pair : ui_pair;
	bool is_def = c == stgs.colors->def;
	int y = top_left.y, x = top_left.x, s = MENU_TILE_SIZE;

	/* Draw tile */
	wattrset(menuw, is_def ? bg_pair : tile_pair);
	draw_square(menuw, top_left, s);

	/* Draw frame */
	wattrset(menuw, is_def ? frame_pair : fg_pair);
	mvwhline(menuw, y,     x,     CHAR_FULL, s);
	mvwvline(menuw, y,     x,     CHAR_FULL, s);
	mvwhline(menuw, y+s-1, x,     CHAR_FULL, s);
	mvwvline(menuw, y,     x+s-1, CHAR_FULL, s);

	/* Draw direction arrow */
	if (!is_def) {
		wattrset(menuw, tile_pair | A_REVERSE);
		mvwaddch(menuw, y+s/2, x+s/2, turn2arrow(stgs.colors->turn[c]));
	}
}

static void draw_color_arrow(Vector2i pos1, Vector2i pos2)
{
	int ts = MENU_TILE_SIZE, o = MENU_TILE_SIZE/2, dy, dx;

	assert(pos1.y != pos2.y || pos1.x != pos2.x);
	wattrset(menuw, fg_pair);

	if (pos1.x == pos2.x) {
		dy = abs(pos1.y - pos2.y) - ts;
		mvwvline(menuw, MIN(pos1.y, pos2.y)+ts, pos1.x+o, ACS_VLINE, dy);
		if (pos1.y < pos2.y) {
			mvwaddch(menuw, pos2.y-1,  pos1.x+o, ACS_DARROW);
		} else {
			mvwaddch(menuw, pos2.y+ts, pos1.x+o, ACS_UARROW);
		}
	} else if (pos1.y == pos2.y) {
		dx = abs(pos1.x - pos2.x);
		dy = MENU_TILE_V_PAD;
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

static void draw_color_rules(void)
{
	color_t c;
	unsigned i = 0;
	bool do_for = TRUE;
	Vector2i pos1, pos2, cdef_pos;

	pos1.y = rules_pos.y - 3;
	pos1.x = rules_pos.x - MENU_TILE_PWIDTH;
	wattrset(menuw, bg_pair);
	draw_rect(menuw, pos1, MENU_TILES_WIDTH, MENU_TILES_HEIGHT);

	if (!stgs.colors) {
		return;
	}

	/* Draw color tiles */
	for (c = stgs.colors->first; do_for; c = stgs.colors->next[c]) {
		pos1 = pos2 = menu_tile_pos(i++);
		if (c == COLOR_NONE) {
			break;
		}
		if (i < MENU_TILES_COUNT) {
			pos2 = menu_tile_pos(i);
			draw_color_arrow(pos1, pos2);
		}
		draw_color_tile(pos1, c);
		state_map[c] = i;
		do_for = c != stgs.colors->last;
	}

	/* Draw placeholder tile */
	if (i < MENU_TILES_COUNT) {
		draw_color_tile(pos2, stgs.colors->def);
	}

	/* Draw arrow back to first tile */
	if (i >= MENU_TILES_PER_COL) {
		draw_color_arrow(pos2, menu_tile_pos(0));
	} else {
		pos1.y = pos2.y;
		pos1.x = pos2.x - MENU_TILE_PWIDTH;
		draw_color_arrow(pos2, pos1);
		pos1.y += MENU_TILE_SIZE+1;
		draw_color_arrow(pos1, menu_tile_pos(0));
	}

	/* Draw default color picker message */
	cdef_pos = menu_cdef_pos();
	wattrset(menuw, PAIR_FOR(MENU_ACTIVE_COLOR));
	mvwaddstr(menuw, cdef_pos.y, cdef_pos.x, dialog_cdef_msg);
}

static void draw_init_size(void)
{
	wattrset(menuw, PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_UP),   menu_isize_u_pos, FALSE);
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_DOWN), menu_isize_d_pos, FALSE);

	wattrset(menuw, fg_pair);
	draw_sprite(menuw, ui_sprite(UI_DIGIT, stgs.init_size), isize_pos, TRUE);
}

static void draw_dir_arrow(void)
{
	wattrset(menuw, fg_pair);
	mvwaddch(menuw, menu_dir_u_pos.y+3, menu_dir_u_pos.x+1, dir2arrow(stgs.simulation->ant->dir));
}

static void draw_direction(void)
{
	wattrset(menuw, PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_UP),    menu_dir_u_pos, FALSE);
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_RIGHT), menu_dir_r_pos, FALSE);
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_DOWN),  menu_dir_d_pos, FALSE);
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_LEFT),  menu_dir_l_pos, FALSE);
	draw_dir_arrow();
}

static void draw_stepup(void)
{
	wattrset(menuw, PAIR_FOR(
		has_enough_colors(stgs.colors) ? MENU_ACTIVE_COLOR : MENU_INACTIVE_COLOR
	));
	draw_sprite(menuw, ui_sprite(UI_STEPUP, 0), menu_stepup_pos, FALSE);
}

static void draw_speed(void)
{
	int mult = MENU_SPEED_HEIGHT / 8;
	int dy = menu_speed_d_pos.y - menu_speed_u_pos.y - 2/mult;
	Vector2i slider_pos = { speed_pos.y + dy - mult*stgs.speed, speed_pos.x };

	wattrset(menuw, bg_pair);
	draw_rect(menuw, speed_pos, SPRITE_DIGIT_WIDTH, MENU_SPEED_HEIGHT+SPRITE_DIGIT_HEIGHT-1);

	/* Draw scrollbar */
	wattrset(menuw, ui_pair);
	mvwvline(menuw, menu_speed_u_pos.y+2, menu_speed_u_pos.x+1, ACS_VLINE, dy);

	/* Draw slider */
	wattrset(menuw, fg_pair);
	mvwvline(menuw, slider_pos.y+1, slider_pos.x-3, CHAR_FULL, 3);

	/* Draw speed value */
	draw_sprite(menuw, ui_sprite(UI_DIGIT, stgs.speed), slider_pos, FALSE);

	/* Draw arrow buttons */
	wattrset(menuw, PAIR_FOR(MENU_ACTIVE_COLOR));
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_UP),   menu_speed_u_pos, FALSE);
	draw_sprite(menuw, ui_sprite(UI_ARROW, DIR_DOWN), menu_speed_d_pos, FALSE);
}

static void draw_state_func(void)
{
	Simulation *sim = stgs.simulation;
	char q[4], str[8];
	color_t ant_color = GRID_ANT_COLOR(sim->grid, sim->ant);
	color_t next_color = sim->colors->next[ant_color];  // Uses sim->colors instead of stgs.colors

	wattrset(menuw, PAIR_FOR(MENU_ACTIVE_COLOR));
	sprintf(q, "q%u", state_map[ant_color]);
	sprintf(str, "f(%3s, ", q);
	mvwaddstr(menuw, func_pos.y, func_pos.x, str);
	waddch(menuw, CHAR_VISIBLE(ant_color));
	waddstr(menuw, " ) = ");

	sprintf(q, "q%u", state_map[next_color]);
	sprintf(str, "(%3s, ", q);
	mvwaddstr(menuw, func_pos.y+1, func_pos.x+1, str);
	waddch(menuw, CHAR_VISIBLE(next_color));
	sprintf(str, ", %c )", turn2arrow(sim->colors->turn[ant_color]));
	waddstr(menuw, str);
}

static void draw_control_buttons(void)
{
	Vector2i o = {
		.y = (MENU_BUTTON_HEIGHT - SPRITE_BUTTON_HEIGHT) / 2,
		.x = (MENU_BUTTON_WIDTH  - SPRITE_BUTTON_WIDTH)  / 2
	};
	Vector2i pos1 = { menu_play_pos.y + o.y, menu_play_pos.x + o.x };
	Vector2i pos2 = { menu_stop_pos.y + o.y, menu_stop_pos.x + o.x };

	wattrset(menuw, ui_pair);
	draw_rect(menuw, menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	draw_rect(menuw, menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

	if (is_simulation_running(stgs.simulation)) {
		wattrset(menuw, PAIR_FOR(MENU_PAUSE_COLOR));
		draw_sprite(menuw, ui_sprite(UI_BUTTON, UIB_PAUSE), pos1, FALSE);
	} else {
		wattrset(menuw, PAIR_FOR(
			has_enough_colors(stgs.colors) ? MENU_PLAY_COLOR : MENU_INACTIVE_COLOR
		));
		draw_sprite(menuw, ui_sprite(UI_BUTTON, UIB_PLAY), pos1, FALSE);
	}

	if (has_simulation_started(stgs.simulation)) {
		wattrset(menuw, PAIR_FOR(MENU_STOP_COLOR));
		draw_sprite(menuw, ui_sprite(UI_BUTTON, UIB_STOP), pos2, FALSE);
	} else {
		wattrset(menuw, PAIR_FOR(
			!is_colors_empty(stgs.colors) ? MENU_CLEAR_COLOR : MENU_INACTIVE_COLOR
		));
		draw_sprite(menuw, ui_sprite(UI_BUTTON, UIB_CLEAR), pos2, FALSE);
	}
}

static void draw_io_button(Vector2i pos, const char **label, IOStatus status, bool draw_status)
{
	Vector2i inner_pos  = { pos.y+1, pos.x+1 };
	Vector2i text_pos   = { pos.y+2, pos.x+2 };
	Vector2i sprite_pos = { pos.y+1, pos.x+3 };

	wattrset(menuw, ui_pair);
	draw_rect(menuw, pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
	wattrset(menuw, bg_pair);
	draw_rect(menuw, inner_pos, MENU_BUTTON_WIDTH-2, MENU_BUTTON_HEIGHT-2);

	wattrset(menuw, fg_pair);
	if (status == STATUS_PENDING) {
		draw_sprite(menuw, ui_sprite(UI_BUTTON, UIB_PENDING), sprite_pos, FALSE);
	} else {
		for (int i = 0; i < MENU_BUTTON_HEIGHT-4; i++) {
			mvwaddnstr(menuw, text_pos.y+i, text_pos.x, label[i], MENU_BUTTON_WIDTH-4);
		}
	}

	if (draw_status) {
		chtype ch = (status == STATUS_SUCCESS) ? PAIR_FOR(COLOR_LIME) | CHAR_FULL
		          : (status == STATUS_FAILURE) ? PAIR_FOR(COLOR_RED)  | CHAR_FULL
		          : CHAR_SEMI;
		mvwvline(menuw, pos.y, pos.x+MENU_BUTTON_WIDTH, ch, MENU_BUTTON_HEIGHT);
	}
}

static void draw_io_buttons(void)
{
#if !SAVE_ENABLE && GALLERY_MODE
	draw_io_button(menu_load_pos, load_label, load_status, FALSE);
#else
	draw_io_button(menu_load_pos, load_label, load_status, TRUE);
#endif
#if SAVE_ENABLE
	draw_io_button(menu_save_pos, save_label, save_status, TRUE);
#endif
}

static void draw_size(void)
{
	Simulation *sim = stgs.simulation;
	unsigned size = sim ? sim->grid->size : 0;
	char str[MENU_COL_WIDTH+1];
	sprintf(str, "%" STR(MENU_COL_WIDTH) "u", size);
	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, size_pos.y, size_pos.x, str);
}

static void draw_steps(void)
{
	Simulation *sim = stgs.simulation;
	Vector2i pos = steps_pos;
	char digits[MENU_STEPS_LEN+1], *d;
	unsigned steps = sim ? sim->steps : 0;
	unsigned len = steps ? (unsigned)log10(steps)+1 : 0;

	wattrset(menuw, fg_pair);
	if (len > MENU_STEPS_LEN) {
		pos.x += MENU_STEPS_LEN*(SPRITE_DIGIT_WIDTH+1) - SPRITE_INFINITY_WIDTH - 1;
		draw_sprite(menuw, ui_sprite(UI_INFINITY, 0), pos, TRUE);
		return;
	}

	sprintf(digits, "%" STR(MENU_STEPS_LEN) "u", steps);
	for (d = digits; d < digits + MENU_STEPS_LEN; d++) {
		if (*d != ' ') {
			int digit = *d - '0';
			wattrset(menuw, fg_pair);
			draw_sprite(menuw, ui_sprite(UI_DIGIT, digit), pos, TRUE);
		} else {
			wattrset(menuw, bg_pair);
			draw_rect(menuw, pos, SPRITE_DIGIT_WIDTH, SPRITE_DIGIT_HEIGHT);
		}
		pos.x += SPRITE_DIGIT_WIDTH+1;
	}
}

static void draw_labels(void)
{
	wattrset(menuw, fg_pair);
	mvwaddstr(menuw, rules_msg_pos.y,  rules_msg_pos.x,  rules_msg);
	mvwaddstr(menuw, isize_msg_pos.y,  isize_msg_pos.x,  isize_msg);
	mvwaddstr(menuw, dir_msg_pos.y,    dir_msg_pos.x,    dir_msg);
	mvwaddstr(menuw, stepup_msg_pos.y, stepup_msg_pos.x, stepup_msg);
	mvwaddstr(menuw, speed_msg_pos.y,  speed_msg_pos.x,  speed_msg);
	mvwaddstr(menuw, func_msg_pos.y,   func_msg_pos.x,   func_msg);
	mvwaddstr(menuw, size_msg_pos.y,   size_msg_pos.x,   size_msg);
	mvwaddstr(menuw, steps_msg_pos.y,  steps_msg_pos.x,  steps_msg);
}

void draw_menu_full(void)
{
	draw_logo();
	draw_border();
	draw_color_rules();
	draw_init_size();
	draw_direction();
	draw_stepup();
	draw_speed();
	draw_state_func();
	draw_control_buttons();
	draw_io_buttons();
	draw_size();
	draw_steps();
	draw_labels();
	wnoutrefresh(menuw);

	if (dialogw) {
		draw_dialog();
	}
}

void draw_menu_iter(void)
{
	static bool sparse;
	Simulation *sim = stgs.simulation;

	if (!sparse && is_grid_sparse(sim->grid)) {
		sparse = TRUE;
		draw_border();
	}
	draw_dir_arrow();
	draw_state_func();
	draw_steps();
	wnoutrefresh(menuw);
}
