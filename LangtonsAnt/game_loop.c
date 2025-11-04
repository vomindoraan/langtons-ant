#include "graphics.h"
#include "serial.h"

static volatile bool do_loop = true;

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
	if (key == KEY_MOUSE && getmouse(mouse) != ERR && mouse->bstate) {
#if MOUSE_ACT_ON_PRESS
		if (mouse->bstate & MOUSE_ANTIMASK) {
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

#define DRAW_ITER(w, ...)              \
	if (! w##_changed) {               \
		draw_##w##_iter(__VA_ARGS__);  \
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

	while (do_loop) {
		state_t input = handle_input(sim);
		bool grid_changed   = input & STATE_GRID_CHANGED;
		bool menu_changed   = input & STATE_MENU_CHANGED;
		bool colors_changed = input & STATE_COLORS_CHANGED;
		sim = stgs.simulation;  // May have changed on input

		curr_time = timer_micros();
		do_step = (curr_time - step_time >= LOOP_STEP_TIME_US(stgs.speed));
		do_menu = (curr_time - menu_time >= LOOP_FRAME_TIME_US*stgs.speed/2);
		do_draw = (curr_time - draw_time >= LOOP_FRAME_TIME_US);

		if (is_simulation_running(sim)) {
			if (do_step) {
				Vector2i prev_pos = sim->ant->pos;
				if (simulation_step(sim)) {
					DRAW_ITER(grid, sim->grid, sim->ant, prev_pos);
				} else {
					grid_changed = menu_changed = true;  // Grid expanded/sparse
				}
				step_time = curr_time;
			}
			if (do_menu) {
				DRAW_ITER(menu);
				menu_time = curr_time;
			}
		}

		if (grid_changed) {
			draw_grid_full(sim->grid, sim->ant);
		}
		if (menu_changed) {
			draw_menu_full();
			do_draw |= !!(pending_action.func);  // Draw before blocking I/O
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
	do_loop = false;
}
