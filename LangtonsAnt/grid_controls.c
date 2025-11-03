#include "graphics.h"

#include <assert.h>

state_t grid_key_command(Grid *grid, Ant *ant, int key, MEVENT *mouse)
{
	Vector2i center = { grid->size/2, grid->size/2 };
	Vector2i pos = abs2rel(ant->pos, center);
	Vector2i tl = abs2rel(grid->top_left, center), br = abs2rel(grid->bottom_right, center);
	int o = GRID_VIEW_SIZE/2 - 1;

	switch (key) {
		/* Scroll - arrow keys */
	case KEY_UP:
		scroll_by(grid, -SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_DOWN:
		scroll_by(grid,  SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_LEFT:
		scroll_by(grid, 0, -SCROLL_STEP_MEDIUM);
		break;
	case KEY_RIGHT:
		scroll_by(grid, 0,  SCROLL_STEP_MEDIUM);
		break;

		/* Scroll - numpad keys */
	case '8':
#ifdef PDCURSES
	case KEY_A2:
#endif
		scroll_by(grid, -SCROLL_STEP_LARGE, 0);
		break;
	case '2':
#ifdef PDCURSES
	case KEY_C2:
#endif
		scroll_by(grid,  SCROLL_STEP_LARGE, 0);
		break;
	case '4':
#ifdef PDCURSES
	case KEY_B1:
#endif
		scroll_by(grid, 0, -SCROLL_STEP_LARGE);
		break;
	case '6':
#ifdef PDCURSES
	case KEY_B3:
#endif
		scroll_by(grid, 0,  SCROLL_STEP_LARGE);
		break;
	case '7':
#ifdef PDCURSES
	case KEY_A1:
#endif
		scroll_by(grid, -SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '9':
#ifdef PDCURSES
	case KEY_A3:
#endif
		scroll_by(grid, -SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;
	case '1':
#ifdef PDCURSES
	case KEY_C1:
#endif
		scroll_by(grid,  SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '3':
#ifdef PDCURSES
	case KEY_C3:
#endif
		scroll_by(grid,  SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;

		/* Jump to bounds */
	case KEY_PPAGE:
		scroll_set(grid, tl.y+o, gridscrl.x);
		break;
	case KEY_NPAGE:
		scroll_set(grid, br.y-o, gridscrl.x);
		break;
	case KEY_HOME:
		scroll_set(grid, gridscrl.y, tl.x+o);
		break;
	case KEY_END:
		scroll_set(grid, gridscrl.y, br.x-o);
		break;

		/* Jump to ant */
	case 'F': case 'f': case '5':
#ifdef PDCURSES
	case KEY_B2:
#endif
		scroll_set(grid, pos.y, pos.x);
		break;

		/* Jump to center */
	case 'C': case 'c': case '0':
#ifdef PDCURSES
	case PAD0:
#endif
		scroll_set(grid, 0, 0);
		break;

	case KEY_MOUSE:
		return grid_mouse_command(grid, ant, mouse);

	default:
		return STATE_NO_CHANGE;
	}

	return STATE_GRID_CHANGED;
}

typedef enum {
	SB_VERTICAL,
	SB_HORIZONTAL
} ScrollbarType;

static void scrollbar_clicked(Grid *grid, MEVENT *mevent, ScrollbarType sbtype)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	Vector2i pos = { mevent->y, mevent->x }, rel;

	if (mevent->bstate & MOUSE_LB_EVENT) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid+gridscrl.vcenter, GRID_VIEW_SIZE });
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid+gridscrl.hcenter });
		}
		scroll_by(grid, SGN(rel.y)*step, SGN(rel.x)*step);
	} else if (mevent->bstate & MOUSE_RB_EVENT) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid, GRID_VIEW_SIZE });
			scroll_set(grid, (int)(rel.y / gridscrl.scale), gridscrl.x);
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid });
			scroll_set(grid, gridscrl.y, (int)(rel.x / gridscrl.scale));
		}
	}
}

state_t grid_mouse_command(Grid *grid, Ant *ant, MEVENT *mouse)
{
	Vector2i center = { grid->size / 2, grid->size / 2 };
	Vector2i mouse_pos, pos;
	bool lb_clicked;
	int step;

	if (!mouse) {
		return STATE_NO_CHANGE;
	}
	mouse_pos.y = mouse->y, mouse_pos.x = mouse->x;
	if (!area_contains(grid_pos, GRID_WINDOW_SIZE, GRID_WINDOW_SIZE, mouse_pos)) {
		return STATE_NO_CHANGE;
	}

	lb_clicked = !!(mouse->bstate & MOUSE_LB_EVENT);
	step = lb_clicked ? SCROLL_STEP_SMALL : grid->size;  // Can be anything large

	/* Vertical scrollbar */
	if (mouse->x == GRID_VIEW_SIZE && mouse->y < GRID_VIEW_SIZE) {
		if (mouse->y == 0) {
			scroll_by(grid, -step, 0);
		} else if (mouse->y == GRID_VIEW_SIZE-1) {
			scroll_by(grid,  step, 0);
		} else {
			scrollbar_clicked(grid, mouse, SB_VERTICAL);
		}
		return STATE_GRID_CHANGED;
	}

	/* Horizontal scrollbar */
	if (mouse->y == GRID_VIEW_SIZE && mouse->x < GRID_VIEW_SIZE) {
		if (mouse->x == 0) {
			scroll_by(grid, 0, -step);
		} else if (mouse->x == GRID_VIEW_SIZE-1) {
			scroll_by(grid, 0,  step);
		} else {
			scrollbar_clicked(grid, mouse, SB_HORIZONTAL);
		}
		return STATE_GRID_CHANGED;
	}

	/* Grid proper - jump to ant/center */
	pos = abs2rel(lb_clicked ? ant->pos : center, center);
	scroll_set(grid, pos.y, pos.x);
	return STATE_GRID_CHANGED;
}
