#include "graphics.h"

#include <assert.h>

input_t grid_key_command(Grid *grid, Ant *ant, int key, MEVENT *pmouse)
{
	Vector2i center = { grid->size/2, grid->size/2 };
	Vector2i pos = abs2rel(ant->pos, center);
	Vector2i tl = abs2rel(grid->top_left, center), br = abs2rel(grid->bottom_right, center);
	int o = GRID_VIEW_SIZE/2 - 1;

	switch (key) {
		/* Scroll - arrow keys */
	case KEY_UP:
		scroll_grid(grid, -SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_DOWN:
		scroll_grid(grid,  SCROLL_STEP_MEDIUM, 0);
		break;
	case KEY_LEFT:
		scroll_grid(grid, 0, -SCROLL_STEP_MEDIUM);
		break;
	case KEY_RIGHT:
		scroll_grid(grid, 0,  SCROLL_STEP_MEDIUM);
		break;

		/* Scroll - numpad keys */
	case '8':
#ifdef PDCURSES
	case KEY_A2:
#endif
		scroll_grid(grid, -SCROLL_STEP_LARGE, 0);
		break;
	case '2':
#ifdef PDCURSES
	case KEY_C2:
#endif
		scroll_grid(grid,  SCROLL_STEP_LARGE, 0);
		break;
	case '4':
#ifdef PDCURSES
	case KEY_B1:
#endif
		scroll_grid(grid, 0, -SCROLL_STEP_LARGE);
		break;
	case '6':
#ifdef PDCURSES
	case KEY_B3:
#endif
		scroll_grid(grid, 0,  SCROLL_STEP_LARGE);
		break;
	case '7':
#ifdef PDCURSES
	case KEY_A1:
#endif
		scroll_grid(grid, -SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '9':
#ifdef PDCURSES
	case KEY_A3:
#endif
		scroll_grid(grid, -SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;
	case '1':
#ifdef PDCURSES
	case KEY_C1:
#endif
		scroll_grid(grid,  SCROLL_STEP_LARGE, -SCROLL_STEP_LARGE);
		break;
	case '3':
#ifdef PDCURSES
	case KEY_C3:
#endif
		scroll_grid(grid,  SCROLL_STEP_LARGE,  SCROLL_STEP_LARGE);
		break;

		/* Jump to bounds */
	case KEY_PPAGE:
		set_scroll(grid, tl.y+o, gridscrl.x);
		break;
	case KEY_NPAGE:
		set_scroll(grid, br.y-o, gridscrl.x);
		break;
	case KEY_HOME:
		set_scroll(grid, gridscrl.y, tl.x+o);
		break;
	case KEY_END:
		set_scroll(grid, gridscrl.y, br.x-o);
		break;

		/* Jump to ant */
	case 'F': case 'f': case '5':
#ifdef PDCURSES
	case KEY_B2:
#endif
		set_scroll(grid, pos.y, pos.x);
		break;

		/* Jump to center */
	case 'C': case 'c': case '0':
#ifdef PDCURSES
	case PAD0:
#endif
		set_scroll(grid, 0, 0);
		break;

	case KEY_MOUSE:
		assert(pmouse);
		return grid_mouse_command(grid, pmouse);

	default:
		return INPUT_NO_CHANGE;
	}

	return INPUT_GRID_CHANGED;
}

typedef enum { SB_VERTICAL, SB_HORIZONTAL } ScrollbarType;

static void scrollbar_clicked(Grid *grid, MEVENT *mevent, ScrollbarType sbtype)
{
	int n = GRID_VIEW_SIZE, mid = n/2, step = n-2;
	Vector2i pos = { mevent->y, mevent->x }, rel;

	if (mevent->bstate & BUTTON1_CLICKED) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid+gridscrl.vcenter, GRID_VIEW_SIZE });
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid+gridscrl.hcenter });
		}
		scroll_grid(grid, sgn(rel.y)*step, sgn(rel.x)*step);
	} else if (mevent->bstate & BUTTON3_CLICKED) {
		if (sbtype == SB_VERTICAL) {
			rel = abs2rel(pos, (Vector2i) { mid, GRID_VIEW_SIZE });
			set_scroll(grid, (int)(rel.y/gridscrl.scale), gridscrl.x);
		} else {
			rel = abs2rel(pos, (Vector2i) { GRID_VIEW_SIZE, mid });
			set_scroll(grid, gridscrl.y, (int)(rel.x/gridscrl.scale));
		}
	}
}

input_t grid_mouse_command(Grid *grid, MEVENT *pmouse)
{
	int step;

	if (!pmouse) {
		return INPUT_NO_CHANGE;
	}

	step = (pmouse->bstate & BUTTON1_CLICKED) ? SCROLL_STEP_SMALL
	     : (pmouse->bstate & BUTTON3_CLICKED) ? grid->size // Can be anything large
	     : 0;

	/* Vertical scrollbar */
	if (pmouse->x == GRID_VIEW_SIZE && pmouse->y < GRID_VIEW_SIZE) {
		if (pmouse->y == 0) {
			scroll_grid(grid, -step, 0);
		} else if (pmouse->y == GRID_VIEW_SIZE-1) {
			scroll_grid(grid,  step, 0);
		} else {
			scrollbar_clicked(grid, pmouse, SB_VERTICAL);
		}
		return INPUT_GRID_CHANGED;
	/* Horizontal scrollbar */
	} else if (pmouse->y == GRID_VIEW_SIZE && pmouse->x < GRID_VIEW_SIZE) {
		if (pmouse->x == 0) {
			scroll_grid(grid, 0, -step);
		} else if (pmouse->x == GRID_VIEW_SIZE-1) {
			scroll_grid(grid, 0,  step);
		} else {
			scrollbar_clicked(grid, pmouse, SB_HORIZONTAL);
		}
		return INPUT_GRID_CHANGED;
	}

	return INPUT_NO_CHANGE;
}
