#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Colors *load_colors(const char *filename)  // TODO format checks
{
	Colors *colors;
	FILE *input;
	int e;
	color_t def, c;

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	e = fscanf(input, "%hd\n", &def);
	if (def < 0 || def >= COLOR_COUNT) {
		return NULL;
	}
	colors = colors_new(def);
	colors->def = def;

	for (c = 0; c < COLOR_COUNT; c++) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next+c);
	}
	for (c = 0; c < COLOR_COUNT; c++) {
		e += fscanf(input, (c == COLOR_COUNT-1) ? "%c\n" : "%c ", colors->turn+c);
	}
	e += fscanf(input, "%hd %hd\n", &colors->first, &colors->last);
	e += fscanf(input, "%zu\n", &colors->n);

	if (fclose(input) == EOF) {
		return NULL;
	}

	if (e < COLORS_TOTAL_FIELDS) {
		colors_delete(colors);
		return NULL;
	}

	return colors;
}

int save_colors(const char *filename, Colors *colors)
{
	FILE *output;
	int e;
	color_t c;

	if (!(output = fopen(filename, "w"))) {
		return EOF;
	}

	e = fprintf(output, "%hd\n", colors->def);
	for (c = 0; c < COLOR_COUNT; c++) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%hd\n" : "%hd ", colors->next[c]);
	}
	for (c = 0; c < COLOR_COUNT; c++) {
		e += fprintf(output, (c == COLOR_COUNT-1) ? "%c\n" : "%c ", colors->turn[c]);
	}
	e += fprintf(output, "%hd %hd\n", colors->first, colors->last);
	e += fprintf(output, "%zu\n", colors->n);

	if (fclose(output) == EOF || e < COLORS_TOTAL_FIELDS) {
		return EOF;
	}
	return e;
}

Simulation *load_simulation(const char *filename)
{
	Simulation *sim;
	Colors *colors;
	FILE *input;
	size_t skip = 0, i, j;
	bool is_sparse;

	if (!(colors = load_colors(filename))) {
		return NULL;
	}

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	while (skip < 5) {
		int c = getc(input);
		if (c == '\n') {
			skip++;
		}
	}

	sim = simulation_new(colors, GRID_DEF_INIT_SIZE);

	if (fscanf(input, "%d %d %u\n", &sim->ant->pos.x, &sim->ant->pos.y,
		       &sim->ant->dir) < 3) {
		return sim;  // Colors only
	}
	if (fscanf(input, "%zu\n", &sim->steps) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%c\n", &is_sparse) < 0) {
		goto error_end;
	}

	grid_delete(sim->grid);
	sim->grid = malloc(sizeof(Grid));
	sim->grid->c = NULL;
	sim->grid->tmp = NULL;
	sim->grid->tmp_size = 0;
	sim->grid->csr = NULL;

	if (fscanf(input, "%hhu %zu %zu %zu\n", &sim->grid->def_color,
		       &sim->grid->init_size, &sim->grid->size, &sim->grid->colored) < 0) {
		goto error_end;
	}
	if (fscanf(input, "%d %d %d %d\n", &sim->grid->top_left.x, &sim->grid->top_left.y,
		       &sim->grid->bottom_right.x, &sim->grid->bottom_right.y) < 0) {
		goto error_end;
	}

	if (is_sparse) {
		sim->grid->csr = malloc(sim->grid->size * sizeof(SparseCell *));
		size_t colp;
		SparseCell *cell;

		for (i = 0; i < sim->grid->size; i++) {
			char c;
			cell = NULL;
			sim->grid->csr[i] = NULL;
			while (fscanf(input, "%c", &c) > 0 && c == ' ') {
				if (fscanf(input, "%zu", &colp) < 0) {
					goto error_end;
				}
				if (!cell) {
					sim->grid->csr[i] = malloc(sizeof(SparseCell));
					sim->grid->csr[i]->packed = colp;
					sim->grid->csr[i]->next = NULL;
					cell = sim->grid->csr[i];
				} else {
					cell->next = malloc(sizeof(SparseCell));
					cell->next->packed = colp;
					cell->next->next = NULL;
					cell = cell->next;
				}
			}
		}
	} else {
		sim->grid->c = malloc(sim->grid->size * sizeof(byte *));
		for (i = 0; i < sim->grid->size; i++) {
			sim->grid->c[i] = malloc(sim->grid->size);
			if (feof(input)) {
				goto error_end;
			}
			for (j = 0; j < sim->grid->size; j++) {
				if (fscanf(input, (j == sim->grid->size - 1) ? "%hhu\n" : "%hhu ",
					       &sim->grid->c[i][j]) < 0) {
					goto error_end;
				}
			}
		}
	}

	fclose(input);
	return sim;

error_end:
	simulation_delete(sim);
	fclose(input);
	return NULL;
}

int save_simulation(const char *filename, Simulation *sim)
{
	FILE *output;
	SparseCell *cell;
	size_t i, j;

	if (save_colors(filename, sim->colors) == EOF) {
		return EOF;
	}

	if (!(output = fopen(filename, "a"))) {
		return EOF;
	}

	if (fprintf(output, "%d %d %u\n", sim->ant->pos.x, sim->ant->pos.y,
		        sim->ant->dir) < 0) {
		return EOF;
	}
	if (fprintf(output, "%zu\n", sim->steps) < 0) {
		return EOF;
	}
	if (fprintf(output, "%c\n", is_grid_sparse(sim->grid)) < 0) {
		return EOF;
	}
	if (fprintf(output, "%hhu %zu %zu %zu\n", sim->grid->def_color,
		        sim->grid->init_size, sim->grid->size, sim->grid->colored) < 0) {
		return EOF;
	}
	if (fprintf(output, "%d %d %d %d\n", sim->grid->top_left.x, sim->grid->top_left.y,
		        sim->grid->bottom_right.x, sim->grid->bottom_right.y) < 0) {
		return EOF;
	}

	if (is_grid_sparse(sim->grid)) {
		for (i = 0; i < sim->grid->size; i++) {
			cell = sim->grid->csr[i];
			while (cell) {
				if (fprintf(output, " %zu", cell->packed) < 0) {
					return EOF;
				}
				cell = cell->next;
			}
			if (fprintf(output, "\n") < 0) {
				return EOF;
			}
		}
	} else {
		for (i = 0; i < sim->grid->size; i++) {
			for (j = 0; j < sim->grid->size; j++) {
				if (fprintf(output, (j == sim->grid->size-1) ? "%hhu\n" : "%hhu ",
					        sim->grid->c[i][j]) < 0) {
					return EOF;
				}
			}
		}
	}

	if (fclose(output) == EOF) {
		return EOF;
	}
	return 0;  // TODO return success bool
}

int save_grid_bitmap(const char *filename, Grid *grid)
{
	pixel_t *image;
	size_t height = grid->size, width = grid->size, i, j;

	image = malloc(height * width * sizeof(pixel_t));
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			Vector2i pos = (Vector2i) { j, i };  // Flip axes
			color_t color = GRID_COLOR_AT(grid, pos);
			memcpy(image[i*width + j], color_map[color], sizeof(pixel_t));
		}
	}

	int e = create_bitmap_file(filename, image, height, width);
	free(image);
	return e;
}
