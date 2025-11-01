#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Colors *load_colors(const char *filename)
{
	Colors *colors;
	FILE *input;
	int e;
	color_t def, i, c;
	turn_t t;

	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}

	e = fscanf(input, "%hd\n", &def);
	if (def < 0 || def >= COLOR_COUNT) {
		return NULL;
	}
	def = BGR(def);
	colors = colors_new(def);
	colors->def = def;

	for (i = 0; i < COLOR_COUNT; i++) {
		e += fscanf(input, (i < COLOR_COUNT-1) ? "%hd " : "%hd\n", &c);
		colors->next[BGR(i)] = BGR(c);
	}
	for (i = 0; i < COLOR_COUNT; i++) {
		e += fscanf(input, (i < COLOR_COUNT-1) ? "%hhd " : "%hhd\n", &t);
		colors->turn[BGR(i)] = t;
	}
	e += fscanf(input, "%hd %hd\n", &i, &c);
	colors->first = BGR(i), colors->last = BGR(c);
	e += fscanf(input, "%u\n", &colors->n);

	if (fclose(input) == EOF || e < COLORS_FIELD_COUNT) {
		colors_delete(colors);
		return NULL;
	}
	return colors;
}

int save_colors(const char *filename, Colors *colors)
{
	FILE *output;
	int e;
	color_t i;

	if (!(output = fopen(filename, "w"))) {
		return EOF;
	}

	e = fprintf(output, "%hd\n", BGR(colors->def));
	for (i = 0; i < COLOR_COUNT; i++) {
		e += fprintf(output, (i < COLOR_COUNT-1) ? "%hd " : "%hd\n",
		             BGR(colors->next[BGR(i)]));
	}
	for (i = 0; i < COLOR_COUNT; i++) {
		e += fprintf(output, (i < COLOR_COUNT-1) ? "%hhd " : "%hhd\n",
		             colors->turn[BGR(i)]);
	}
	e += fprintf(output, "%hd %hd\n", BGR(colors->first), BGR(colors->last));
	e += fprintf(output, "%u\n", colors->n);

	if (fclose(output) == EOF || e < COLORS_FIELD_COUNT) {
		return EOF;
	}
	return e;
}

typedef int (*io_func_t)(Simulation *, FILE *);

static int load_cells_n(Simulation *sim, FILE *input) {
	unsigned i, j;
	sim->grid->c = malloc(sim->grid->size * sizeof(byte *));

	for (i = 0; i < sim->grid->size; i++) {
		if (feof(input)) {
			return EOF;
		}
		sim->grid->c[i] = malloc(sim->grid->size);

		for (j = 0; j < sim->grid->size; j++) {
			byte c;
			if (fscanf(input, (j < sim->grid->size-1) ? "%hhu " : "%hhu\n", &c) < 1) {
				return EOF;
			}
			sim->grid->c[i][j] = BGR(c);
		}
	}
	return 0;
}

static int save_cells_n(Simulation *sim, FILE *output)
{
	unsigned i, j;
	for (i = 0; i < sim->grid->size; i++) {
		for (j = 0; j < sim->grid->size; j++) {
			byte c = BGR(sim->grid->c[i][j]);
			if (fprintf(output, (j < sim->grid->size-1) ? "%hhu " : "%hhu\n", c) < 0) {
				return EOF;
			}
		}
	}
	return 0;
}

static int load_cells_s(Simulation *sim, FILE *input) {
	SparseCell cell = { 0 }, *sc = &cell;
	unsigned i;
	sim->grid->csr = calloc(sim->grid->size, sizeof(SparseCell *));

	for (i = 0; i < sim->grid->size; i++) {
		SparseCell *p = NULL;
		char c;

		while (TRUE) {
			if (fscanf(input, "%c", &c) < 1) {
				return EOF;
			}
			if (c == '\n' || (c == '\r' && fgetc(input) == '\n')) {
				break;  // newline, end of row
			}
			if (fscanf(input, "%X", &cell.packed) < 1) {
				return EOF;
			}

			CSR_SET_COLOR(sc, BGR(CSR_GET_COLOR(sc)));
			p = sparse_append(p, CSR_GET_COLUMN(sc), CSR_GET_COLOR(sc));
			if (!sim->grid->csr[i]) {
				sim->grid->csr[i] = p;
			}
		}
	}
	return 0;
}

static int save_cells_s(Simulation *sim, FILE *output)
{
	unsigned i;
	for (i = 0; i < sim->grid->size; i++) {
		SparseCell *curr = sim->grid->csr[i];
		while (curr) {
			SparseCell cell = *curr;
			CSR_SET_COLOR(&cell, BGR(CSR_GET_COLOR(&cell)));

			if (fprintf(output, " %08X", cell.packed) < 0) {
				return EOF;
			}
			curr = curr->next;
		}

		if (fprintf(output, "\n") < 0) {
			return EOF;
		}
	}
	return 0;
}

Simulation *load_simulation(const char *filename)
{
	Simulation *sim;
	Colors *colors;
	FILE *input;
	bool is_sparse;
	byte def, skip;
	io_func_t load_cells;

	if (!(colors = load_colors(filename))) {
		return NULL;
	}
	if (!(input = fopen(filename, "r"))) {
		return NULL;
	}
	for (skip = 0; skip < 5; skip += (getc(input) == '\n'));

	sim = simulation_new(colors, GRID_DEF_INIT_SIZE);
	if (fscanf(input, "%d %d %u\n", &sim->ant->pos.y, &sim->ant->pos.x,
	           &sim->ant->dir) < 3) {
		return sim;  // Colors only
	}
	if (fscanf(input, "%u\n", &sim->steps) < 1) {
		goto error_end;
	}
	if (fscanf(input, "%hhu\n", &is_sparse) < 1) {
		goto error_end;
	}

	grid_delete(sim->grid);
	sim->grid = malloc(sizeof(Grid));
	sim->grid->c = NULL;
	sim->grid->tmp = NULL;
	sim->grid->tmp_size = 0;
	sim->grid->csr = NULL;
	if (fscanf(input, "%hhu %u %u %u\n", &def,
	           &sim->grid->init_size, &sim->grid->size, &sim->grid->colored) < 4) {
		goto error_end;
	}
	sim->grid->def_color = BGR(def);
	if (fscanf(input, "%d %d %d %d", &sim->grid->top_left.y, &sim->grid->top_left.x,
	           &sim->grid->bottom_right.y, &sim->grid->bottom_right.x) < 4) {
		goto error_end;
	}
	if (fgetc(input) == '\r' && fgetc(input) != '\n') {  // Read trailing newline
		goto error_end;
	}

	load_cells = is_sparse ? load_cells_s : load_cells_n;
	if (load_cells(sim, input) == EOF) {
		goto error_end;
	}

	fclose(input);
	return sim;

error_end:
	fclose(input);
	simulation_delete(sim);
	return NULL;
}

int save_simulation(const char *filename, Simulation *sim)
{
	FILE *output;
	io_func_t save_cells;

	if (save_colors(filename, sim->colors) == EOF) {
		return EOF;
	}
	if (!(output = fopen(filename, "a"))) {
		return EOF;
	}

	if (fprintf(output, "%d %d %u\n", sim->ant->pos.y, sim->ant->pos.x,
	            sim->ant->dir) < 0) {
		goto error_end;
	}
	if (fprintf(output, "%u\n", sim->steps) < 0) {
		goto error_end;
	}
	if (fprintf(output, "%hhu\n", is_grid_sparse(sim->grid)) < 0) {
		goto error_end;
	}
	if (fprintf(output, "%hhu %u %u %u\n", BGR(sim->grid->def_color),
	            sim->grid->init_size, sim->grid->size, sim->grid->colored) < 0) {
		goto error_end;
	}
	if (fprintf(output, "%d %d %d %d\n", sim->grid->top_left.y, sim->grid->top_left.x,
	            sim->grid->bottom_right.y, sim->grid->bottom_right.x) < 0) {
		goto error_end;
	}

	save_cells = is_grid_sparse(sim->grid) ? save_cells_s : save_cells_n;
	if (save_cells(sim, output) == EOF) {
		goto error_end;
	}

	return fclose(output);

error_end:
	fclose(output);
	return EOF;
}

int save_grid_bitmap(const char *filename, Grid *grid)
{
	pixel_t *image;
	unsigned height = grid->size, width = grid->size, i, j;
	size_t size = width * height * sizeof(pixel_t);

	if (size > BMP_MAX_SZ || !(image = malloc(size))) {
		return EOF;
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			Vector2i pos = (Vector2i) { height-i-1, j };
			const pixel_t *pixel = color_map + GRID_COLOR_AT(grid, pos);
			memcpy(image[i*width + j], pixel, sizeof(pixel_t));
		}
	}

	int e = create_bitmap_file(filename, image, height, width);
	free(image);
	return e;
}
