#include "graphics.h"
#ifdef SERIAL_COLORS
#	include "serial.h"
#endif

static bool run_loop = TRUE;

static state_t handle_input(Simulation *sim)
{
	state_t ret;
	int key;
	MEVENT m, *mouse = &m;

	// These functions must be called only once per loop
	if ((key = getch()) == ERR) {
		return STATE_NO_CHANGE;
	}
	if (key == KEY_MOUSE && getmouse(mouse) != ERR) {
#if defined(MOUSE_ACT_ON_PRESS) && defined(NCURSES)
		if (mouse->bstate & (BUTTON1_RELEASED | BUTTON3_RELEASED)) {
			return STATE_NO_CHANGE; // Prevent double press
		}
#endif
	} else {
		mouse = NULL;
	}

	ret = STATE_NO_CHANGE;
	if (sim) {
		ret |= grid_key_command(sim->grid, sim->ant, key, mouse);
	}
	ret |= menu_key_command(key, mouse);
	return ret;
}

void game_loop(void)
{
	Simulation *sim = stgs.linked_sim;
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		state_t input = handle_input(sim);
		state_t grid_changed = input & STATE_GRID_CHANGED;
		state_t menu_changed = input & STATE_MENU_CHANGED;
#ifdef SERIAL_COLORS
		state_t colors_changed = input & STATE_COLORS_CHANGED;
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
#ifdef SERIAL_COLORS
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
