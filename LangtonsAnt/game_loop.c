#include "graphics.h"
#include "serial.h"

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

void game_loop(void)
{
	static ttime_t step_time, menu_time, draw_time;
	ttime_t curr_time;
	bool do_step, do_draw, do_menu;
	Simulation *sim = stgs.simulation;

	init_timer();
	draw_grid_full(sim->grid, sim->ant);
	draw_menu_full();

	while (run_loop) {
		state_t input = handle_input(sim);
		bool grid_changed   = !!(input & STATE_GRID_CHANGED);
		bool menu_changed   = !!(input & STATE_MENU_CHANGED);
		bool colors_changed = !!(input & STATE_COLORS_CHANGED);
		sim = stgs.simulation;

		curr_time = get_time_us();
		do_step = (curr_time - step_time >= LOOP_STEP_TIME_US(stgs.speed));
		do_menu = (curr_time - menu_time >= LOOP_FRAME_TIME_US*stgs.speed/2 + 1);
		do_draw = (curr_time - draw_time >= LOOP_FRAME_TIME_US);

		if (do_step && is_simulation_running(sim)) {
			Vector2i prev_pos = sim->ant->pos;
			if (simulation_step(sim)) {
				draw_grid_iter(sim->grid, sim->ant, prev_pos);
			} else {
				grid_changed = menu_changed = TRUE;
			}
			step_time = curr_time;
		}

		if (grid_changed) {
			draw_grid_full(sim->grid, sim->ant);
		}
		if (menu_changed) {
			draw_menu_full();
		} else if (do_menu) {
			draw_menu_iter();
			menu_time = curr_time;
		}

		if (do_draw) {
			doupdate();
			draw_time = curr_time;
		}

		if (colors_changed) {
#if SERIAL_COLORS
			serial_send_colors(sim->colors);
#endif
		}
	}
}

void stop_game_loop(void)
{
	run_loop = FALSE;
}
