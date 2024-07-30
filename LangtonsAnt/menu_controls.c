#include "graphics.h"
#include "io.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

//#define INPUT_WINDOW_WIDTH  (MENU_WINDOW_WIDTH-4)
//#define INPUT_WINDOW_HEIGHT 3

static WINDOW *iow;
//static const Vector2i io_pos = { MENU_CONTROLS_Y-22, GRID_WINDOW_SIZE+MENU_WINDOW_WIDTH-INPUT_WINDOW_WIDTH-2 };

static const char* example_files[] = {
	"examples/highway.lant",
	"examples/spiral.lant",
	"examples/triangle.lant",
	"examples/sword.lant",
	"examples/square.lant",
	"examples/zigzag.lant",
	"examples/cauliflower.lant",
	"examples/square3.lant",
};

state_t set_simulation(Simulation *sim)
{
	assert(stgs.linked_sim);
	simulation_delete(stgs.linked_sim);
	stgs.linked_sim = sim;
	colors_delete(stgs.colors);
	stgs.colors = sim->colors;
	stgs.init_size = sim->grid->init_size;
	reset_scroll();
	return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED;
}

state_t reset_simulation(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim) {
		simulation_delete(sim);
	}
	stgs.linked_sim = simulation_new(stgs.colors, stgs.init_size);
	reset_scroll();
	return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED;
}

state_t clear_simulation(void)
{
	colors_clear(stgs.colors);
	return reset_simulation();
}

static state_t isize_button_clicked(int d)
{
	Simulation *sim = stgs.linked_sim;
	if (d > 0) {
		stgs.init_size = MIN(stgs.init_size+d, GRID_MAX_INIT_SIZE);
	} else if (d < 0) {
		stgs.init_size = MAX(stgs.init_size+d, GRID_MIN_INIT_SIZE);
	} else {
		return STATE_NO_CHANGE;
	}
	if (!is_simulation_running(sim) && !has_simulation_started(sim)) { // Sanity check
		return reset_simulation();
	}
	return STATE_MENU_CHANGED;
}

static state_t dir_button_clicked(Direction dir)
{
	stgs.linked_sim->ant->dir = dir;
	return STATE_GRID_CHANGED | STATE_MENU_CHANGED;
}

static state_t speed_button_clicked(int delta)
{
	if (delta > 0) {
		stgs.speed = MIN(stgs.speed+delta, LOOP_MAX_SPEED);
	} else if (delta < 0) {
		stgs.speed = MAX(stgs.speed+delta, LOOP_MIN_SPEED);
	} else {
		return STATE_NO_CHANGE;
	}
	return STATE_MENU_CHANGED;
}

static state_t stepup_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (sim && has_enough_colors(sim->colors)) {
		simulation_halt(sim);
		simulation_step(sim);
		return STATE_GRID_CHANGED | STATE_MENU_CHANGED;
	}
	return STATE_NO_CHANGE;
}

static state_t play_button_clicked(void)
{
	Simulation *sim = stgs.linked_sim;
	if (is_simulation_running(sim)) {
		simulation_halt(sim);
		return STATE_MENU_CHANGED;
	}
	if (sim && has_enough_colors(sim->colors)) {
		simulation_run(sim);
		return STATE_MENU_CHANGED;
	}
	return STATE_NO_CHANGE;
}

static state_t stop_button_clicked(void)
{
	return has_simulation_started(stgs.linked_sim) ? reset_simulation() : clear_simulation();
}

static state_t load_sim_action(void *arg)
{
	const char *filename = arg;
	Simulation *sim;
	if (sim = load_simulation(filename)) {
		load_status = STATUS_SUCCESS;
		return set_simulation(sim);
	} else {
		load_status = STATUS_FAILURE;
		return STATE_MENU_CHANGED;
	}
}

static state_t load_button_clicked(void)
{
	static int index = 0;
	pending_action.func = load_sim_action;
	pending_action.arg = (void *)example_files[index];
	index = (index + 1) % LEN(example_files);
	load_status = STATUS_PENDING;
	return STATE_MENU_CHANGED;
}

//static bool read_filename(const char *filename)
//{
//	iow = newwin(3, INPUT_WINDOW_WIDTH, io_pos.y, io_pos.x); // TODO move to window drawing file
//	wbkgd(iow, GET_PAIR_FOR(COLOR_GRAY) | A_REVERSE);
//	wattron(iow, fg_pair);
//	waddstr(iow, " Path: ");
//	wattroff(iow, fg_pair);
//	echo();
//	mvwgetnstr(iow, 1, 1, filename, FILENAME_SIZE-1);
//	noecho();
//	delwin(iow);
//	return strlen(filename) > 0 && strlen(filename)+4 < FILENAME_SIZE;
//}

//static state_t save_button_clicked(void)
//{
//	char filename[FILENAME_SIZE] = { 0 }, bmp_filename[FILENAME_SIZE] = { 0 };
//	if (read_filename(filename) && save_simulation(filename, stgs.linked_sim) != EOF) {
//		save_status = STATUS_SUCCESS;
//		strcpy(bmp_filename, filename);
//		strcat(bmp_filename, ".bmp");
//		save_grid_bitmap(bmp_filename, stgs.linked_sim->grid);
//	} else {
//		save_status = STATUS_FAILURE;
//	}
//	return STATE_MENU_CHANGED;
//}

state_t menu_key_command(int key, MEVENT *mouse)
{
	Simulation *sim = stgs.linked_sim;

	switch (key) {
		/* Init size */
	case ']':
		return isize_button_clicked(1);
	case '[':
		return isize_button_clicked(-1);

		/* Ant direction */
	case 'W': case 'w':
		return dir_button_clicked(DIR_UP);
	case 'D': case 'd':
		return dir_button_clicked(DIR_RIGHT);
	case 'S': case 's':
		return dir_button_clicked(DIR_DOWN);
	case 'A': case 'a':
		return dir_button_clicked(DIR_LEFT);

		/* Speed */
	case 'Q': case 'q': case '=':
#ifdef PDCURSES
	case PADPLUS:
#endif
		return speed_button_clicked(+1);
	case 'Z': case 'z': case '-':
#ifdef PDCURSES
	case PADMINUS:
#endif
		return speed_button_clicked(-1);

		/* Step+ */
	case 'E': case 'e': case '.':
#ifdef PDCURSES
	case PADSTOP:
#endif
		return stepup_button_clicked();

		/* Control */
	case ' ': case '\n':
#ifdef PDCURSES
	case PADENTER:
#endif
		return play_button_clicked();
	case 'R': case 'r':
		return stop_button_clicked();
	case 'X': case 'x': case '\b':
		return clear_simulation();

		/* IO */
	case KEY_F(1):
		return load_button_clicked();
	//case KEY_F(2):
	//	return save_button_clicked();

		/* Quit */
	case KEY_ESC:
		stop_game_loop();
		return STATE_NO_CHANGE;

	case KEY_MOUSE:
		//assert(mouse);
		return menu_mouse_command(mouse);

	default:
		return STATE_NO_CHANGE;
	}
}

state_t menu_mouse_command(MEVENT *mouse)
{
	state_t ret = STATE_NO_CHANGE;
	Vector2i mouse_pos, pos, tile;
	size_t i;

	if (!mouse) {
		return STATE_NO_CHANGE;
	}
	mouse_pos.y = mouse->y, mouse_pos.x = mouse->x;
	if (!area_contains(menu_pos, MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT, mouse_pos)) {
		return STATE_NO_CHANGE;
	}

	if (dialogw) {
		if (area_contains(dialog_pos, DIALOG_WINDOW_WIDTH, DIALOG_WINDOW_HEIGHT, mouse_pos)) {
			return dialog_mouse_command(mouse);
		} else {
			close_dialog();
			return STATE_MENU_CHANGED;
		}
	}

	pos = abs2rel(mouse_pos, menu_pos);

	/* Color tiles */
	for (i = 0; i <= stgs.colors->n; i++) {
		tile = get_menu_tile_pos(i);
		if (area_contains(tile, MENU_TILE_SIZE, MENU_TILE_SIZE, pos)) {
			if (mouse->bstate & MOUSE_LB_EVENT) {
				open_dialog(pos, (i == stgs.colors->n) ? CIDX_NEWCOLOR : (color_t)i);
			} else if (mouse->bstate & MOUSE_RB_EVENT) {
				open_dialog(pos, CIDX_DEFAULT);
			}
			return STATE_MENU_CHANGED;
		}
	}
	if (area_contains(get_menu_cdef_pos(), strlen(dialog_cdef_msg), 1, pos)) {
		open_dialog(pos, CIDX_DEFAULT);
		return STATE_MENU_CHANGED;
	}

	/* Init size buttons */
	if (area_contains(menu_isize_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return isize_button_clicked(1);
	}
	if (area_contains(menu_isize_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return isize_button_clicked(-1);
	}

	/* Ant direction buttons */
	if (area_contains(menu_dir_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return dir_button_clicked(DIR_UP);
	}
	if (area_contains(menu_dir_r_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, pos)) {
		return dir_button_clicked(DIR_RIGHT);
	}
	if (area_contains(menu_dir_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return dir_button_clicked(DIR_DOWN);
	}
	if (area_contains(menu_dir_l_pos, MENU_RLARROW_WIDTH, MENU_RLARROW_HEIGHT, pos)) {
		return dir_button_clicked(DIR_LEFT);
	}

	/* Speed buttons */
	if (area_contains(menu_speed_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return speed_button_clicked(+1);
	}
	if (area_contains(menu_speed_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return speed_button_clicked(-1);
	}

	/* Step+ button */
	if (area_contains(menu_stepup_pos, MENU_STEPUP_SIZE, MENU_STEPUP_SIZE, pos)) {
		return stepup_button_clicked();
	}

	/* Control buttons */
	if (area_contains(menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return play_button_clicked();
	}
	if (area_contains(menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return stop_button_clicked();
	}

	/* IO buttons */
	if (area_contains(menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return load_button_clicked();
	}
	//if (area_contains(menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
	//	return save_button_clicked();
	//}

	return ret;
}
