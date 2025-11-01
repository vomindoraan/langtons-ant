#include "logic.h"

#include <assert.h>
#include <stdlib.h>

Simulation *simulation_new(Colors *colors, unsigned init_size)
{
	assert(colors);
	Simulation *sim = malloc(sizeof(Simulation));
	sim->colors = colors;
	sim->grid = grid_new(colors, init_size);
	sim->ant = ant_new(sim->grid, DIR_UP);
	sim->steps = 0;
	sim->is_running = FALSE;
	return sim;
}

void simulation_delete(Simulation *sim)
{
	assert(sim);
	grid_delete(sim->grid);
	ant_delete(sim->ant);
	free(sim);
}

void simulation_run(Simulation *sim)
{
	assert(sim);
	sim->is_running = TRUE;
}

void simulation_halt(Simulation *sim)
{
	assert(sim);
	sim->is_running = FALSE;
}

bool simulation_step(Simulation *sim)
{
	assert(sim);
	bool was_sparse = is_grid_sparse(sim->grid);
	bool in_bounds = ant_move(sim->ant, sim->grid, sim->colors);
	grid_silent_expand(sim->grid);
	if (!in_bounds) {
		grid_expand(sim->grid, sim->ant);
	}
	sim->steps++;
	return in_bounds && was_sparse == is_grid_sparse(sim->grid);
}

bool is_simulation_running(Simulation *sim)
{
	return sim && sim->is_running;
}

bool has_simulation_started(Simulation *sim)
{
	return sim && sim->steps > 0;
}
