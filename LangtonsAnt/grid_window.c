#include "graphics.h"

#include <assert.h>
#include <stdlib.h>

WINDOW         *gridw;
ScrollInfo      gridscrl;
const Vector2i  grid_pos = { 0, 0 };

void init_grid_window(void)
{
	gridw = newwin(GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, grid_pos.y, grid_pos.x);
	wbkgd(gridw, ui_pair);
	keypad(gridw, TRUE);
	nodelay(gridw, TRUE);
}

void end_grid_window(void)
{
	delwin(gridw);
	gridw = NULL;
}

static Vector2i pos2yx(Vector2i pos, int line_width, int cell_size, int offset)
{
	return (Vector2i) {
		.y = offset + line_width*(pos.y+1) + cell_size*pos.y,
		.x = offset + line_width*(pos.x+1) + cell_size*pos.x,
	};
}

static void draw_buffer_zone(int total, int offset)
{
	int n = GRID_WINDOW_SIZE, i;
	for (i = 0; i < offset; i++) {
		mvwhline(gridw, i,     i,     CHAR_FULL, n-i);
		mvwvline(gridw, i,     i,     CHAR_FULL, n-i);
		mvwhline(gridw, n-1-i, i,     CHAR_FULL, n-i);
		mvwvline(gridw, i,     n-1-i, CHAR_FULL, n-i);
	}
	if ((n - total - 2*offset) % 2) {
		mvwhline(gridw, n-1-i, i,     CHAR_FULL, n-i);
		mvwvline(gridw, i,     n-1-i, CHAR_FULL, n-i);
	}
}


static bool draw_cell(Vector2i yx, int cs, color_t c, Ant *ant)
{
	/* Ignore if cell isn't visible */
	if (yx.y < 0 || yx.y >= GRID_VIEW_SIZE || yx.x < 0 || yx.x >= GRID_VIEW_SIZE) {
		return FALSE;
	}

	wattrset(gridw, PAIR_FOR(c));
	draw_square(gridw, yx, cs);

	if (ant) {
		Vector2i center = { yx.y + cs/2, yx.x + cs/2 };
		SpriteInfo sprite = ant_sprite(cs, ant->dir);
		if (sprite.data) {
			yx.y = center.y - sprite.height/2;
			yx.x = center.x - sprite.width/2;
			wattrset(gridw, fg_pair);
			draw_sprite(gridw, sprite, yx, FALSE);
		} else {
			mvwaddch(gridw, center.y, center.x, dir2arrow(ant->dir) | A_REVERSE);
		}
	}

	return TRUE;
}

static void draw_scrollbars(color_t def)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	int size = (int)MAX(step * gridscrl.scale, 1);
	int h = mid + gridscrl.hcenter - size/2;
	int v = mid + gridscrl.vcenter - size/2;
	chtype sb_fg_pair = AVAILABLE_PAIR(def, COLOR_WHITE, COLOR_SILVER);
	chtype sb_bg_pair = AVAILABLE_PAIR(def, COLOR_GRAY,  COLOR_SILVER);

	h -= h > mid;
	v -= v > mid;

	/* Scrollbar background */
	wattrset(gridw, sb_bg_pair);
	mvwhline(gridw, n, 0, CHAR_FULL, n);
	mvwvline(gridw, 0, n, CHAR_FULL, n);

	/* Scrollbar arrows */
	wattron(gridw, A_REVERSE);
	mvwaddch(gridw, 0,   n,   ACS_UARROW);
	mvwaddch(gridw, n-1, n,   ACS_DARROW);
	mvwaddch(gridw, n,   0,   ACS_LARROW);
	mvwaddch(gridw, n,   n-1, ACS_RARROW);

	/* Scrollbar sliders */
	wattrset(gridw, sb_fg_pair);
	mvwhline(gridw, n, h, CHAR_FULL, size);
	mvwvline(gridw, v, n, CHAR_FULL, size);
}

static void bordered(Grid *grid, Ant *ant, int line_width)
{
	int gs = grid->size, i, j;
	int cs = CELL_SIZE(gs, line_width);
	int t = TOTAL_SIZE(gs, line_width, cs);
	int o = OFFSET_SIZE(t);
	Vector2i pos, yx;

	/* Draw background edge buffer zone */
	wattrset(gridw, bg_pair);
	draw_buffer_zone(t, o);

	/* Draw grid lines */
	wattrset(gridw, fg_pair);
	for (i = 0; i < GRID_WINDOW_SIZE; i += cs+line_width) {
		for (j = 0; j < line_width; j++) {
			mvwhline(gridw, o+i+j, o,     CHAR_FULL, t);
			mvwvline(gridw, o,     o+i+j, CHAR_FULL, t);
		}
	}

	/* Draw cells */
	for (i = 0; i < gs; i++) {
		for (j = 0; j < gs; j++) {
			pos.y = i, pos.x = j;
			yx = pos2yx(pos, line_width, cs, o);
			draw_cell(yx, cs, GRID_COLOR_AT(grid, pos),
			          (ant && VECTOR_EQ(pos, ant->pos)) ? ant : NULL);
		}
	}
}

static void borderless(Grid *grid, Ant *ant)
{
	int gs = grid->size, vgs = MIN(gs, GRID_VIEW_SIZE), i, j;
	int cs = CELL_SIZE(vgs, 0);
	int t = TOTAL_SIZE(vgs, 0, cs);
	int o = OFFSET_SIZE(t);
	Vector2i rel, pos, yx, origin = grid_pos;

	/* Draw background edge buffer zone */
	wattrset(gridw, PAIR_FOR(grid->def_color));
	draw_buffer_zone(t, o);

	/* Draw scrollbars in case of largest grid */
	gridscrl.enabled = gs > vgs;
	if (gridscrl.enabled) {
		origin = ORIGIN_POS(gs, vgs, gridscrl.y, gridscrl.x);
		gridscrl.scale = GRID_VIEW_SIZE / (double)gs;
		gridscrl.hcenter = (int)(gridscrl.scale * gridscrl.x);
		gridscrl.vcenter = (int)(gridscrl.scale * gridscrl.y);
		draw_scrollbars(grid->def_color);
	}

	/* Draw cells */
	for (i = 0; i < vgs; i++) {
		for (j = 0; j < vgs; j++) {
			rel.y = i, rel.x = j;
			yx = pos2yx(rel, 0, cs, o);
			pos = rel2abs(rel, origin);
			draw_cell(yx, cs, GRID_COLOR_AT(grid, pos),
			          (ant && VECTOR_EQ(pos, ant->pos)) ? ant : NULL);
		}
	}
}

void draw_grid_full(Grid *grid, Ant *ant)
{
	if (grid) {
		if (grid->size == GRID_SIZE_SMALL(grid)) {
			bordered(grid, ant, LINE_WIDTH_SMALL);
		} else if (grid->size == GRID_SIZE_MEDIUM(grid)) {
			bordered(grid, ant, LINE_WIDTH_MEDIUM);
		} else {
			assert(IS_GRID_LARGE(grid));
			borderless(grid, ant);
		}
	} else {
		wbkgd(gridw, ui_pair);
	}
	wnoutrefresh(gridw);
}

void draw_grid_iter(Grid *grid, Ant *ant, Vector2i prev_pos)
{
	int gs = grid->size, vgs = MIN(gs, GRID_VIEW_SIZE);
	int lw = (gs == (int)GRID_SIZE_SMALL(grid))  ? LINE_WIDTH_SMALL
	       : (gs == (int)GRID_SIZE_MEDIUM(grid)) ? LINE_WIDTH_MEDIUM
	       : LINE_WIDTH_LARGE;
	int cs = CELL_SIZE(vgs, lw);
	int o = OFFSET_SIZE(TOTAL_SIZE(vgs, lw, cs));
	Vector2i origin = ORIGIN_POS(gs, vgs, gridscrl.y, gridscrl.x), pos, yx;
	bool drawn;

	/* Redraw cell at previous position */
	pos = abs2rel(prev_pos, origin);
	yx = pos2yx(pos, lw, cs, o);
	drawn = draw_cell(yx, cs, GRID_COLOR_AT(grid, prev_pos), NULL);

	/* Draw cell at ant's position */
	if (ant) {
		pos = abs2rel(ant->pos, origin);
		yx = pos2yx(pos, lw, cs, o);
		drawn |= draw_cell(yx, cs, GRID_ANT_COLOR(grid, ant), ant);
	}

	if (drawn) {
		wnoutrefresh(gridw);
	}
}

void scroll_by(Grid *grid, int dy, int dx)
{
	scroll_set(grid, gridscrl.y+dy, gridscrl.x+dx);
}

void scroll_set(Grid *grid, int y, int x)
{
	int gs = grid->size, n = GRID_VIEW_SIZE, clamp = MAX(gs/2 - n/2, 0);

	if (!gridscrl.enabled) {
		return;
	}

	y = SGN(y) * MIN(abs(y), clamp);
	x = SGN(x) * MIN(abs(x), clamp);

	gridscrl.y = y, gridscrl.x = x;
	gridscrl.hcenter = (int)(gridscrl.scale * x);
	gridscrl.vcenter = (int)(gridscrl.scale * y);
}

void scroll_reset(void)
{
	if (gridscrl.enabled) {
		gridscrl.y = gridscrl.x = 0;
		gridscrl.hcenter = gridscrl.vcenter = 0;
		gridscrl.scale = 0.0;
	}
}
