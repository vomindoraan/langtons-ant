#include "graphics.h"
#include "io.h"

#include <assert.h>
#include <string.h>

#define INPUT_WINDOW_WIDTH  (MENU_WINDOW_WIDTH - 4)
#define INPUT_WINDOW_HEIGHT 3

static WINDOW* inputw;
static const Vector2i input_pos = {
	.y = MENU_CONTROLS_Y - 13,
	.x = GRID_WINDOW_SIZE + MENU_WINDOW_WIDTH - INPUT_WINDOW_WIDTH - 2,
};

static bool read_filename(char* filename)
{
	int ret;
	inputw = newwin(3, INPUT_WINDOW_WIDTH, input_pos.y, input_pos.x);  // TODO: Move to window drawing file
	wbkgd(inputw, PAIR_FOR(COLOR_GRAY) | A_REVERSE);
	wattron(inputw, fg_pair);
	waddstr(inputw, " Filename: ");
	wattroff(inputw, fg_pair);
	echo();
	ret = mvwgetnstr(inputw, 1, 1, filename, FILENAME_SZ - 5);  // Leave room for ".bmp"
	noecho();
	delwin(inputw);
	return ret != ERR && strlen(filename) > 0;
}

static const char *example_files[] = {
	"examples/highway.lant",
	"examples/spiral.lant",
	"examples/triangle.lant",
	"examples/sword.lant",
	"examples/square.lant",
	"examples/zigzag.lant",
	"examples/cauliflower.lant",
	"examples/square3.lant",
	"examples/test.lant"
};

#if GALLERY_MODE
#	define USER_FILE  example_files[LEN(example_files) - 1]
#endif

state_t set_simulation(Simulation *sim)
{
	assert(stgs.simulation);
	simulation_delete(stgs.simulation);
	stgs.simulation = sim;
	colors_delete(stgs.colors);
	stgs.colors = sim->colors;
	stgs.init_size = sim->grid->init_size;
	scroll_reset();
	return STATE_GRID_CHANGED | STATE_MENU_CHANGED | STATE_COLORS_CHANGED;
}

state_t reset_simulation(void)
{
	Simulation *sim = stgs.simulation;
	if (sim) {
		simulation_delete(sim);
	}
	stgs.simulation = simulation_new(stgs.colors, stgs.init_size);
	scroll_reset();
	return STATE_GRID_CHANGED | STATE_MENU_CHANGED;
}

state_t clear_simulation(void)
{
	colors_clear(stgs.colors);
	return reset_simulation() | STATE_COLORS_CHANGED;
}

static state_t isize_button_clicked(int d)
{
	Simulation *sim = stgs.simulation;
	unsigned old_value = stgs.init_size, ret = 0;
	if (d > 0) {
		stgs.init_size = MIN(stgs.init_size+d, GRID_MAX_INIT_SIZE);
	} else if (d < 0) {
		stgs.init_size = MAX(stgs.init_size+d, GRID_MIN_INIT_SIZE);
	}
	if (stgs.init_size != old_value)  {
		if (!is_simulation_running(sim) && !has_simulation_started(sim)) {  // Sanity check
			Direction dir = sim->ant->dir;
			ret = reset_simulation();
			stgs.simulation->ant->dir = dir;  // Preserve ant direction
		}
		return STATE_MENU_CHANGED | ret;
	}
	return STATE_NO_CHANGE;
}

static state_t dir_button_clicked(Direction dir)
{
	if (dir != stgs.simulation->ant->dir) {
		stgs.simulation->ant->dir = dir;
		return STATE_GRID_CHANGED | STATE_MENU_CHANGED;
	}
	return STATE_NO_CHANGE;
}

static state_t speed_button_clicked(int delta)
{
	unsigned old_value = stgs.speed;
	if (delta > 0) {
		stgs.speed = MIN((int)stgs.speed+delta, LOOP_MAX_SPEED);
	} else if (delta < 0) {
		stgs.speed = MAX((int)stgs.speed+delta, LOOP_MIN_SPEED);
	}
	return (stgs.speed != old_value) ? STATE_MENU_CHANGED : STATE_NO_CHANGE;
}

static state_t stepup_button_clicked(void)
{
	Simulation *sim = stgs.simulation;
	if (sim && has_enough_colors(sim->colors)) {
		simulation_halt(sim);
		simulation_step(sim);
		return STATE_GRID_CHANGED | STATE_MENU_CHANGED;
	}
	return STATE_NO_CHANGE;
}

static state_t play_button_clicked(void)
{
	Simulation *sim = stgs.simulation;
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
	return has_simulation_started(stgs.simulation) ? reset_simulation() : clear_simulation();
}

static inline void set_pending_action(pending_func_t func, const char *filename)
{
	pending_action.func = func;
	pending_action.arg = (void *)filename;
}

static state_t load_sim_action(void *arg)
{
	char *filename = arg;
	Simulation *sim;
	if ((sim = load_simulation(filename))) {
		load_status = STATUS_SUCCESS;
		return set_simulation(sim);
	} else {
		load_status = STATUS_FAILURE;
		return STATE_MENU_CHANGED;
	}
}

static state_t load_example(int index) {
	static int cycle_index = 0;
	if (index < 0) {
		index = cycle_index;
		cycle_index = (cycle_index+1) % LEN(example_files);
	} else if (index >= (int)LEN(example_files)) {
		return STATE_NO_CHANGE;
	}
	set_pending_action(load_sim_action, example_files[index]);
	load_status = STATUS_PENDING;
	return STATE_MENU_CHANGED;
}

static state_t load_button_clicked(bool input)
{
	static char filename[FILENAME_SZ];
	if (input) {
#if GALLERY_MODE
		strcpy(filename, USER_FILE);
#else
		if (read_filename(filename)) {
#endif
			set_pending_action(load_sim_action, filename);
			load_status = STATUS_PENDING;
#if !GALLERY_MODE
		} else {
			load_status = STATUS_FAILURE;
		}
#endif
		return STATE_MENU_CHANGED;
	}
	return load_example(-1);
}

#if SAVE_ENABLE

static state_t save_sim_action(void *arg)
{
	char *filename = arg;
	if (save_simulation(filename, stgs.simulation) != EOF) {
		save_status = STATUS_SUCCESS;
		strcat(filename, ".bmp");
		save_grid_bitmap(filename, stgs.simulation->grid);
	} else {
		save_status = STATUS_FAILURE;
	}
	return STATE_MENU_CHANGED;
}

static state_t save_button_clicked(void)
{
	static char filename[FILENAME_SZ];
#if GALLERY_MODE
	strcpy(filename, USER_FILE);
#else
	if (read_filename(filename)) {
#endif
		set_pending_action(save_sim_action, filename);
		save_status = STATUS_PENDING;
#if !GALLERY_MODE
	} else {
		save_status = STATUS_FAILURE;
	}
#endif
	return STATE_MENU_CHANGED;
}

#endif  // SAVE_ENABLE

state_t menu_key_command(int key, MEVENT *mouse)
{
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

		/* Step+ */
	case 'E': case 'e': case '.':
#ifdef PDCURSES
	case PADSTOP:
#endif
		return stepup_button_clicked();

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

		/* I/O */
	case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		return load_example(key - '1');
	case KEY_F(1): case KEY_F(3):
		return load_button_clicked(key == KEY_F(3));
#if SAVE_ENABLE
	case KEY_F(2):
		return save_button_clicked();
#endif

		/* Quit */
	case KEY_ESC:
		stop_game_loop();
		return STATE_NO_CHANGE;

	case KEY_MOUSE:
		return menu_mouse_command(mouse);

	default:
		return STATE_NO_CHANGE;
	}
}

state_t menu_mouse_command(MEVENT *mouse)
{
	Vector2i mouse_pos, pos, tile;
	bool lb_clicked, rb_clicked;
	unsigned i;

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
	lb_clicked = !!(mouse->bstate & MOUSE_LB_EVENT);
	rb_clicked = !!(mouse->bstate & MOUSE_RB_EVENT);

	/* Logo area */
	if (area_contains(menu_logo_pos, MENU_LOGO_WIDTH, MENU_LOGO_HEIGHT, pos)) {
		menu_cycle_logo();
		return STATE_MENU_CHANGED;
	}

	/* Color tiles */
	for (i = 0; i <= stgs.colors->n; i++) {
		tile = menu_tile_pos(i);
		if (area_contains(tile, MENU_TILE_SIZE, MENU_TILE_SIZE, pos)) {
			if (lb_clicked) {
				open_dialog(pos, (i == stgs.colors->n) ? CIDX_NEWCOLOR : (color_t)i);
			} else if (rb_clicked) {
				open_dialog(pos, CIDX_DEFAULT);
			}
			return STATE_MENU_CHANGED;
		}
	}
	if (area_contains(menu_cdef_pos(), (unsigned)strlen(dialog_cdef_msg), 1, pos)) {
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

	/* Step+ button */
	if (area_contains(menu_stepup_pos, MENU_STEPUP_SIZE, MENU_STEPUP_SIZE, pos)) {
		return stepup_button_clicked();
	}

	/* Speed buttons */
	if (area_contains(menu_speed_u_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return speed_button_clicked(+1);
	}
	if (area_contains(menu_speed_d_pos, MENU_UDARROW_WIDTH, MENU_UDARROW_HEIGHT, pos)) {
		return speed_button_clicked(-1);
	}

	/* Control buttons */
	if (area_contains(menu_play_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return play_button_clicked();
	}
	if (area_contains(menu_stop_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return stop_button_clicked();
	}

	/* I/O buttons */
	if (area_contains(menu_load_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return load_button_clicked(rb_clicked);
	}
#if SAVE_ENABLE
	if (area_contains(menu_save_pos, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT, pos)) {
		return save_button_clicked();
	}
#endif

	return STATE_NO_CHANGE;
}
