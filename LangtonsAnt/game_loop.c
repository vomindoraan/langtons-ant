#include "graphics.h"
#ifdef COLORS_SERIAL
#	include "serial.h"
#endif

static bool run_loop = TRUE;

static input_t handle_input(Simulation *sim)
{
	input_t ret;
	int key;
	MEVENT mouse, *pmouse = NULL;

	// These functions must be called only once per loop
	if ((key = getch()) == ERR) {
		return INPUT_NO_CHANGE;
	}
	if (key == KEY_MOUSE && getmouse(&mouse) != ERR) {
		pmouse = &mouse;
	}

	ret = INPUT_NO_CHANGE;
	if (sim) {
		ret |= grid_key_command(sim->grid, sim->ant, key, pmouse);
	}
	ret |= menu_key_command(key, pmouse);
	return ret;
}

void game_loop(void)
{
	Simulation *sim = stgs.linked_sim;
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		input_t input = handle_input(sim);
		input_t grid_changed = input & INPUT_GRID_CHANGED, menu_changed = input & INPUT_MENU_CHANGED;
#ifdef COLORS_SERIAL
		input_t colors_changed = input & INPUT_COLORS_CHANGED;
#endif
		sim = stgs.linked_sim;

		if (is_simulation_running(sim)) {
			Vector2i prev_pos = sim->ant->pos;
			if (simulation_step(sim)) {
				int delta = (LOOP_MAX_DELAY - LOOP_MIN_DELAY) / (LOOP_MAX_SPEED - LOOP_MIN_SPEED + 1);
				int delay = delta * (LOOP_MAX_SPEED - stgs.speed);
				napms(delay); // TODO fixed timestep loop
				draw_grid_iter(sim->grid, sim->ant, prev_pos);
				draw_menu_iter();
			} else {
				grid_changed = menu_changed = TRUE;
			}
		}

		if (grid_changed) {
			draw_grid_full(sim->grid, sim->ant);
		}
		if (menu_changed) {
			draw_menu_full();
		}
#ifdef COLORS_SERIAL
		if (colors_changed) {
			serial_send_colors(sim->colors);
		}
#endif

		doupdate();
	}
}

void stop_game_loop(void)
{
	run_loop = FALSE;
}
