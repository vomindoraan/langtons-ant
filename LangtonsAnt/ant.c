#include "logic.h"

#include <assert.h>
#include <stdlib.h>

Ant *ant_new(Grid *grid, Direction dir)
{
	Ant *ant = malloc(sizeof(Ant));
	ant->pos.y = ant->pos.x = grid->size / 2;
	ant->dir = dir;
	return ant;
}

void ant_delete(Ant *ant)
{
	free(ant);
}

static Direction change_dir(Ant *ant, turn_t turn)
{
	switch (ant->dir) {
	case DIR_UP:
		ant->pos.x += turn;
		break;
	case DIR_RIGHT:
		ant->pos.y += turn;
		break;
	case DIR_DOWN:
		ant->pos.x -= turn;
		break;
	case DIR_LEFT:
		ant->pos.y -= turn;
		break;
	}
	return ant->dir = (ant->dir + turn + 4) % 4;
}

static void update_bounding_box(Grid *grid, Vector2i pos)
{
	Vector2i *tl = &grid->top_left, *br = &grid->bottom_right;
	tl->y = MIN(tl->y, pos.y);
	tl->x = MIN(tl->x, pos.x);
	br->y = MAX(br->y, pos.y);
	br->x = MAX(br->x, pos.x);
}

static void ant_move_n(Ant *ant, Grid *grid, Colors *colors)
{
	byte *c = &grid->c[ant->pos.y][ant->pos.x];
	bool is_def = *c == colors->def;
	turn_t turn;

	/* In-place color changing */
	if (is_color_special(colors, *c)) {
		*c = (byte)colors->next[*c];
	}

	turn = colors->turn[*c];
	assert(abs(turn) == 1);
	*c = (byte)colors->next[*c];
	change_dir(ant, turn);

	if (is_def) {
		grid->colored++;
		update_bounding_box(grid, ant->pos);
		if (IS_GRID_LARGE(grid) && is_grid_usage_low(grid)) {
			grid_make_sparse(grid);
		}
	}
}

static void ant_move_s(Ant *ant, Grid *grid, Colors *colors)
{
	int y = ant->pos.y, x = ant->pos.x;
	turn_t turn;
	SparseCell **t = grid->csr + y;

	while (*t && CSR_GET_COLUMN(*t) < (unsigned)x) {
		t = &(*t)->next;
	}
	if (!*t || CSR_GET_COLUMN(*t) != (unsigned)x) {
		if (!*t) {
			grid->colored++;
			update_bounding_box(grid, ant->pos);
		}
		sparse_prepend(t, x, (byte)colors->first);
	}

	/* In-place color changing */
	if (is_color_special(colors, CSR_GET_COLOR(*t))) {
		CSR_SET_COLOR(*t, colors->next[CSR_GET_COLOR(*t)]);
	}

	turn = colors->turn[CSR_GET_COLOR(*t)];
	assert(abs(turn) == 1);
	CSR_SET_COLOR(*t, (byte)colors->next[CSR_GET_COLOR(*t)]);
	change_dir(ant, turn);
}

bool ant_move(Ant *ant, Grid *grid, Colors *colors)
{
	if (is_grid_sparse(grid)) {
		ant_move_s(ant, grid, colors);
	} else {
		ant_move_n(ant, grid, colors);
	}
	return is_ant_in_bounds(ant, grid);
}

bool is_ant_in_bounds(Ant *ant, Grid *grid)
{
	return ant->pos.y >= 0 && (unsigned)ant->pos.y < grid->size
	    && ant->pos.x >= 0 && (unsigned)ant->pos.x < grid->size;
}
