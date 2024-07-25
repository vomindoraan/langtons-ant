#include "graphics.h"

#include <assert.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>

HANDLE console;
CONSOLE_FONT_INFOEX user_font;
SMALL_RECT user_window;
#endif

chtype fg_pair, bg_pair, ui_pair, ui_pair_contrast;

void init_def_pairs(color_t fg_color, color_t bg_color)
{
	color_t ui_color = AVAILABLE_COLOR(bg_color, COLOR_WHITE, COLOR_SILVER), c;
	short p;

	for (c = 0; c < COLOR_COUNT; c++) {
		color_t contrast = IS_COLOR_BRIGHT(c) ? fg_color : bg_color;
		p = GET_PAIRNO_FOR(c);
		init_pair(p, c, contrast);
		if (c == bg_color) {
			bg_pair = COLOR_PAIR(p);
		} else if (c == fg_color) {
			fg_pair = COLOR_PAIR(p);
		} else if (c == ui_color) {
			ui_pair_contrast = COLOR_PAIR(p);
		}
	}

	p = GET_PAIRNO_FOR(c);
	init_pair(p, ui_color, bg_color);
	ui_pair = COLOR_PAIR(p);
}

void init_graphics(color_t fg_color, color_t bg_color)
{
#ifdef _WIN32
	/* Store current console font */
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	user_font.cbSize = sizeof(CONSOLE_FONT_INFOEX); // Required for the below call
	GetCurrentConsoleFontEx(console, FALSE, &user_font);

	/* Store current window position & size */
	CONSOLE_SCREEN_BUFFER_INFO tmp;
	GetConsoleScreenBufferInfo(console, &tmp);
	user_window = tmp.srWindow;

	/* Set console font to 8x8 raster */
	CONSOLE_FONT_INFOEX font = {
		.cbSize = sizeof(CONSOLE_FONT_INFOEX),
		.nFont = 0,
		.dwFontSize = { .X = CONSOLE_FONT_SIZE, .Y = CONSOLE_FONT_SIZE },
		.FontFamily = FF_DONTCARE,
		.FontWeight = FW_NORMAL,
	};
	wcscpy(font.FaceName, CONSOLE_FONT_FACE);
	SetCurrentConsoleFontEx(console, FALSE, &font);
#endif

	initscr();
	resize_term(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH);
	curs_set(0);

	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	mousemask(MOUSE_MASK, NULL);
	mouseinterval(0); // React on key press instead of release (click)

	start_color();
	init_def_pairs(fg_color, bg_color);

	init_grid_window();
	init_menu_window();
	if (!gridw || !menuw) {
		printw("Couldn't initialize graphics, terminal font probably too large");
	}

	refresh();
}

void end_graphics(void)
{
	end_grid_window();
	end_menu_window();
	endwin();

#ifdef _WIN32
	/* Restore user console font and window position & size */
	SetCurrentConsoleFontEx(console, FALSE, &user_font);
	SetConsoleWindowInfo(console, TRUE, &user_window);
#endif
}

Vector2i rel2abs(Vector2i rel, Vector2i origin)
{
	return (Vector2i) {
		.y = origin.y + rel.y,
		.x = origin.x + rel.x
	};
}

Vector2i abs2rel(Vector2i abs, Vector2i origin)
{
	return (Vector2i) {
		.y = abs.y - origin.y,
		.x = abs.x - origin.x
	};
}

bool area_contains(Vector2i top_left, size_t width, size_t height, Vector2i v)
{
	return (v.y >= top_left.y && v.y < top_left.y+(int)height
	        && v.x >= top_left.x && v.x < top_left.x+(int)width);
}

void draw_square(WINDOW *w, Vector2i top_left, size_t size)
{
	size_t i;
	if (size == 1) {
		mvwaddch(w, top_left.y, top_left.x, FILL_CHAR);
		return;
	}
	for (i = 0; i < size; i++) {
		mvwhline(w, top_left.y+i, top_left.x, FILL_CHAR, size);
	}
}

void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height)
{
	size_t i;
	if (width == 1 && height == 1) {
		mvwaddch(w, top_left.y, top_left.x, FILL_CHAR);
		return;
	}
	for (i = 0; i < height; i++) {
		mvwhline(w, top_left.y+i, top_left.x, FILL_CHAR, width);
	}
}

void draw_border(WINDOW *w, Vector2i top_left, size_t width, size_t height)
{
	size_t y = height-1, x = width-1, i;
	if (width == 0 || height == 0) {
		return;
	}
	if (width > 1 && height > 1) {
		mvwaddch(w, top_left.y,   top_left.x,   ACS_ULCORNER);
		mvwaddch(w, top_left.y,   top_left.x+x, ACS_URCORNER);
		mvwaddch(w, top_left.y+y, top_left.x,   ACS_LLCORNER);
		mvwaddch(w, top_left.y+y, top_left.x+x, ACS_LRCORNER);
	} else {
		mvwaddch(w, top_left.y,   top_left.x+x, ACS_PLUS);
		mvwaddch(w, top_left.y+y, top_left.x,   ACS_PLUS);
	}
	for (i = 1; i < width-1; i++) {
		mvwaddch(w, top_left.y,   top_left.x+i, ACS_HLINE);
		mvwaddch(w, top_left.y+y, top_left.x+i, ACS_HLINE);
	}
	for (i = 1; i < height-1; i++) {
		mvwaddch(w, top_left.y+i, top_left.x,   ACS_VLINE);
		mvwaddch(w, top_left.y+i, top_left.x+x, ACS_VLINE);
	}
}

void draw_sprite(WINDOW *w, SpriteInfo sprite, Vector2i top_left, bool overwrite)
{
	size_t read, y, x;
	byte pixel;
	for (read = 0; read < sprite.width*sprite.height; read++) {
		pixel = sprite.data[read/8] >> (7-read%8) & 0x1;
		y = read / sprite.width, x = read % sprite.width;
		if (overwrite) {
			mvwaddch(w, top_left.y+y, top_left.x+x, pixel ? FILL_CHAR : ' ');
		} else if (pixel) {
			mvwaddch(w, top_left.y+y, top_left.x+x, FILL_CHAR);
		}
	}
}

chtype dir2arrow(Direction dir)
{
	switch (dir) {
	case DIR_UP:
		return ACS_UARROW;
	case DIR_RIGHT:
		return ACS_RARROW;
	case DIR_DOWN:
		return ACS_DARROW;
	case DIR_LEFT:
		return ACS_LARROW;
	default:
		return assert(0), ' ';
	}
}

chtype turn2arrow(turn_t turn)
{
	switch (turn) {
	case TURN_LEFT:
		return '<';
	case TURN_RIGHT:
		return '>';
	case TURN_NONE:
		return '-';
	default:
		return assert(0), ' ';
	}
}
