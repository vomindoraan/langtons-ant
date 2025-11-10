#include "graphics.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc > 2) {
		fprintf(stderr, "usage: %s [simulation_file]\n", *argv);
		return EXIT_FAILURE;
	}

	stgs.init_size = GRID_DEF_INIT_SIZE;
	stgs.speed = LOOP_DEF_SPEED;
	if (argc == 2 && (stgs.simulation = load_simulation(argv[1]))) {
		stgs.colors = stgs.simulation->colors;
	} else {
		stgs.colors = colors_new(COLOR_SILVER);
		stgs.simulation = simulation_new(stgs.colors, stgs.init_size);
	}

	init_graphics(COLOR_BLACK, COLOR_WHITE);

	main_loop();

	end_graphics();

	simulation_delete(stgs.simulation);
	colors_delete(stgs.colors);
	return EXIT_SUCCESS;
}
