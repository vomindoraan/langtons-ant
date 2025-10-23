#include "logic.h"

#include <assert.h>
#include <stdlib.h>

Colors *colors_new(color_t def)
{
	assert(def >= 0 && def < COLOR_COUNT);
	Colors *colors = malloc(sizeof(Colors));
	int i;
	for (i = 0; i < COLOR_COUNT; i++) {
		colors->next[i] = def;
		colors->turn[i] = TURN_NONE;
	}
	colors->n = 0;
	colors->first = colors->last = COLOR_NONE;
	colors->def = def;
	return colors;
}

void colors_delete(Colors *colors)
{
	free(colors);
}

static void update_def(Colors *c)
{
	c->next[c->def] = c->next[c->first];
	c->turn[c->def] = c->turn[c->first];
}

void colors_push(Colors *colors, color_t c, turn_t turn)
{
	if (c < 0 || c >= COLOR_COUNT || c == colors->def) {
		return;
	}
	
	if (colors->first == COLOR_NONE) {
		assert(colors->last == COLOR_NONE);
		colors->first = c;
		colors->last = c;
	}
	colors->next[colors->last] = c;
	colors->next[c] = colors->first;
	colors->last = c;
	colors->turn[c] = turn;

	update_def(colors);
	colors->n++;
}

void colors_pop(Colors *colors, color_t c)
{
	color_t i;
	if (c < 0 || c >= COLOR_COUNT || c == colors->def || colors->n == 0) {
		return;
	}

	colors->turn[c] = colors->turn[colors->def] = TURN_NONE;
	if (colors->n-- == 1) {
		colors->first = colors->last = COLOR_NONE;
		colors->next[c] = colors->next[colors->def] = colors->def;
		return;
	}

	if (colors->first == c) {
		colors->first = colors->next[colors->last] = colors->next[c];
	}
	for (i = 0; i < COLOR_COUNT; i++) {
		if (colors->next[i] == c) {
			colors->next[i] = colors->next[c];
			if (colors->turn[i] != TURN_NONE && colors->last == c) {
				colors->last = i;
			}
		}
	}
	update_def(colors);
}

void colors_clear(Colors *colors)
{
	color_t i;
	for (i = 0; i < COLOR_COUNT; i++) {
		colors->next[i] = colors->def;
		colors->turn[i] = TURN_NONE;
	}
	colors->first = colors->last = COLOR_NONE;
	colors->n = 0;
}

void colors_update(Colors *colors, unsigned index, color_t c, turn_t turn)
{
	color_t prev = colors->last, i = colors->first, j;
	assert(index < colors->n);
	for (; index > 0; index--) {
		prev = i;
		i = colors->next[i];
	}

	colors->next[prev] = c;
	colors->next[c] = colors->next[i];
	colors->turn[c] = turn;

	/* Special color */
	colors->next[i] = c;
	for (j = 0; j < COLOR_COUNT; j++) {
		if (colors->next[j] == i) {
			colors->next[j] = c;
		}
	}
	colors->turn[i] = TURN_NONE;

	if (i == colors->first) {
		colors->first = c;
	}
	if (i == colors->last) {
		colors->last = c;
	}
	update_def(colors);
}

void colors_set_turn(Colors *colors, unsigned index, turn_t turn)
{
	color_t i = colors->first;
	assert(index < colors->n);
	for (; index > 0; index--) {
		i = colors->next[i];
	}
	colors->turn[i] = turn;
	update_def(colors);
}

color_t colors_at(Colors *colors, unsigned index)
{
	color_t i = colors->first;
	assert(index < colors->n);
	for (; index > 0; index--) {
		i = colors->next[i];
	}
	return i;
}

bool color_exists(Colors *colors, color_t c)
{
	return colors->turn[c] != TURN_NONE;
}

bool is_color_special(Colors *colors, color_t c)
{
	return colors->next[c] != colors->def && colors->turn[c] == TURN_NONE;
}

bool is_colors_empty(Colors *colors)
{
	return colors->n == 0;
}

bool has_enough_colors(Colors *colors)
{
	return colors->n >= 2;
}
