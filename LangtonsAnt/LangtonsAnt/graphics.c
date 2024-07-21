#include "graphics.h"

#include <math.h>

#ifdef _WIN32
#include <Windows.h>

HANDLE console;
CONSOLE_FONT_INFOEX user_font;
SMALL_RECT user_window;
#endif

chtype fg_pair, bg_pair, ui_pair;

void init_def_pairs(color_t fg_color, color_t bg_color)
{
	color_t i;
	for (i = 0; i < COLOR_COUNT; i++) {
		if (i == bg_color) {
			init_pair(i+1, i, fg_color);
			bg_pair = GET_PAIR_FOR(i);
		} else {
			init_pair(i+1, i, bg_color);
			if (i == fg_color) {
				fg_pair = GET_PAIR_FOR(i);
			}
		}
	}
	ui_pair = AVAILABLE_PAIR(bg_color, COLOR_WHITE, COLOR_SILVER);
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
	mousemask(BUTTON1_CLICKED | BUTTON3_CLICKED, NULL); // Left and right click
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	start_color();
	init_def_pairs(fg_color, bg_color);
	
	init_grid_window();
	init_menu_window();

	if (gridw == NULL || menuw == NULL) {
		printw("Couldn't initialize graphics: Terminal font too large");
		wnoutrefresh(stdscr);
	}

	doupdate();
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
		mvwaddch(w, top_left.y, top_left.x, ACS_BLOCK);
		return;
	}
	for (i = 0; i < size; i++) {
		mvwhline(w, top_left.y+i, top_left.x, ACS_BLOCK, size);
	}
}

void draw_rect(WINDOW *w, Vector2i top_left, size_t width, size_t height)
{
	size_t i;
	if (width == 1 && height == 1) {
		mvwaddch(w, top_left.y, top_left.x, ACS_BLOCK);
		return;
	}
	for (i = 0; i < height; i++) {
		mvwhline(w, top_left.y+i, top_left.x, ACS_BLOCK, width);
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
			mvwaddch(w, top_left.y+y, top_left.x+x, pixel ? ACS_BLOCK : ' ');
		} else if (pixel) {
			mvwaddch(w, top_left.y+y, top_left.x+x, ACS_BLOCK);
		}
	}
}

chtype dir2arrow(Direction dir)
{
	static const chtype arrows[] = { ACS_UARROW, ACS_RARROW, ACS_DARROW, ACS_LARROW };
	return arrows[dir];
}

chtype turn2arrow(turn_t turn)
{
	return (turn == TURN_LEFT)  ? '<'
	     : (turn == TURN_RIGHT) ? '>'
	     : '-';
}
