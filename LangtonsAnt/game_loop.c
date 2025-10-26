#include "graphics.h"
#if SERIAL_COLORS
#	include "serial.h"
#endif

#include <stdlib.h>

static bool run_loop = TRUE;

static state_t handle_input(Simulation *sim)
{
	state_t ret;
	int key;
	MEVENT m, *mouse = &m;

	if (pending_action.func) {
		ret = (*pending_action.func)(pending_action.arg);  // Blocking

		flushinp();
		pending_action.func = NULL;
		return ret;
	}

	// These functions must be called only once per loop
	if ((key = getch()) == ERR) {
		return STATE_NO_CHANGE;
	}
	if (key == KEY_MOUSE && getmouse(mouse) != ERR) {
#if defined(MOUSE_ACT_ON_PRESS) && defined(NCURSES)
		if (mouse->bstate & (BUTTON1_RELEASED | BUTTON3_RELEASED)) {
			return STATE_NO_CHANGE;  // Prevent double press
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

// TODO: Fixed timestep loop
static void sleep(void)
{
	int dd = LOOP_MAX_DELAY - LOOP_MIN_DELAY;
	int ds = LOOP_MAX_SPEED - LOOP_MIN_SPEED;
	int x  = LOOP_MAX_SPEED - stgs.speed;
	// Linear delay: dd * x / ds + LOOP_MIN_DELAY
	// Cubic delay:  ceil(dd * x^3 / ds^3) + LOOP_MIN_DELAY
	div_t d = div(dd * (x*x*x), ds*ds*ds);
	int delay = d.quot + (d.rem != 0) + LOOP_MIN_DELAY;
	napms(delay);
}

void game_loop(void)
{
	Simulation *sim = stgs.simulation;
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		state_t input = handle_input(sim);
		bool grid_changed = input & STATE_GRID_CHANGED;
		bool menu_changed = input & STATE_MENU_CHANGED;
#if SERIAL_COLORS
		bool colors_changed = input & STATE_COLORS_CHANGED;
#endif
		sim = stgs.simulation;

		if (is_simulation_running(sim)) {
			Vector2i prev_pos = sim->ant->pos;
			if (simulation_step(sim)) {
				draw_grid_iter(sim->grid, sim->ant, prev_pos);
				draw_menu_iter();
				sleep();
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
#if SERIAL_COLORS
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
