#include "command_window.h"

static void clear_input(char *input, int buffer_size)
{

	for (int i = 0; i < buffer_size; i++)
	{
		input[i] = 0;
	}
}

static void set_cursor_at_command_window(Command_Window *window)
{
	wmove(window->window, window->cursor_y, window->cursor_x);
}

static void clear_command_input(Command_Window *window)
{
	window->input_position = 0;
	clear_input(window->command, window->buffer_size);
	wmove(window->window, 1, 1);
	wclrtoeol(window->window);
}

static void add_to_command_input(Command_Window *window, int input)
{
	if (window->input_position < window->buffer_size - 1)
	{
		window->command[window->input_position++] = input;
	}
}

static void add_string_to_command_input(Command_Window* window, const char* input, size_t command_size) {
	for(size_t i = 0; i < command_size; i++) {
		add_to_command_input(window, input[i]);
	}
}

static void delete_character_command_window(Command_Window *window)
{

	if (window->input_position > 0)
	{
		wmove(window->window, window->cursor_y, window->cursor_x - 1);
		window->command[--window->input_position] = 0;
	}
	else
	{
		window->command[0] = 0;
	}
	wclrtoeol(window->window);
}

static void close_tab(Command_Window *window)
{

	log_info("Closing current tab \n");

	window->callback(window->parent, ":CLOSE_CURRENT_TAB\t");

	set_cursor_at_command_window(window);
}

static void move_to_next_tab(Command_Window *window)
{
	window->callback(window->parent, ":MOVE_TO_NEXT_TAB\t");

	set_cursor_at_command_window(window);
}

static void move_to_previously_tab(Command_Window *window)
{
	window->callback(window->parent, ":MOVE_TO_PREVIOUSLY_TAB\t");

	set_cursor_at_command_window(window);
}

static void resize_app_window(Command_Window *window)
{
	window->callback(window->parent, ":RESIZE_WINDOW\t");
}

Command_Window create_command_window(int parentRows, int parentColumn, int buffer_size, void *parent, Submit_Filter_Callback callback)
{
	Command_Window window = {0};
	WINDOW *newWindow = newwin(3, parentColumn, parentRows - 2, 0);
	keypad(newWindow, TRUE);
	window.window = newWindow;

	window.input_position = 0;
	window.parent = parent;
	window.callback = callback;
	window.columns = parentColumn - 2;
	window.rows = parentRows - 2;

	window.command = malloc(sizeof(char) * buffer_size);
	window.buffer_size = buffer_size;
	window.cursor_x = 1;
	window.cursor_y = 1;

	window.command_history = init_command_history(HISTORY_LOCATION);

	clear_input(window.command, window.buffer_size);
	wrefresh(window.window);
	wmove(window.window, window.cursor_y, window.cursor_x);

	wtimeout(window.window, 100);

	return window;
}

bool handle_input_command_window(Command_Window *window)
{

	int input = wgetch(window->window);

	if (input != -1)
	{
		if (input == KEY_PPAGE)
		{
			window->callback(window->parent, ":GO_UP\t");

			set_cursor_at_command_window(window);
			return false;
		}
		else if (input == KEY_NPAGE)
		{
			window->callback(window->parent, ":GO_DOWN\t");

			set_cursor_at_command_window(window);
			return false;
		}
		else if (input == KEY_RESIZE)
		{
			resize_app_window(window);
			return false;
		}

		log_info("Typed the character with code %d and value %s \n", input, keyname(input));
		// log_info("Typed the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with codeTyped the character with code %d and value %s \n", input, keyname(input));
		if (input == KEY_BACKSPACE || input == KEY_DC || input == 127)
		{
			log_info("Pressed backspace \n");
			delete_character_command_window(window);
		}
		else if (input == KEY_ENTER || input == 10)
		{
			if (window->input_position != 0)
			{
				window->command[window->input_position++] = 0;
				add_to_command_history(&window->command_history, window->command);
				log_info("Is filtering for %s, with size: %d\n", window->command, window->input_position);

				window->callback(window->parent, window->command);

				set_cursor_at_command_window(window);

				clear_command_input(window);
			}

			return false;
		}
		else if (input == ('u' & 0x1F))
		{
			clear_command_input(window);
			return false;
		}
		else if (input == ('w' & 0x1F))
		{
			close_tab(window);
		}
		else if (input == 561)
		{
			log_info("Going to next tab \n");
			move_to_next_tab(window);
			return false;
		}
		else if (input == 546)
		{
			log_info("Going to previously tab \n");
			move_to_previously_tab(window);
			return false;
		}
		else if (input == KEY_UP)
		{
			const char *previous_command = get_previous_command(&window->command_history);

			size_t command_size = strlen(previous_command);

			if (command_size > 0)
			{
				clear_command_input(window);

				add_string_to_command_input(window, previous_command, command_size);
			}
		}
		else if (input == KEY_DOWN)
		{
			const char *next_command = get_next_command(&window->command_history);

			size_t command_size = strlen(next_command);

			if (command_size > 0)
			{
				clear_command_input(window);

				add_string_to_command_input(window, next_command, command_size);
			}
		}
		else
		{
			add_to_command_input(window, input);
		}
		mvwprintw(window->window, 1, 1, "%*.*s", 0, window->columns, window->command);
		return true;
	}

	return false;
}

void resize_command_window(Command_Window* window, int parentRows, int parentColumn)
{
    delwin(window->window);

	WINDOW *newWindow = newwin(3, parentColumn, parentRows - 2, 0);
	keypad(newWindow, TRUE);
	window->window = newWindow;

	window->input_position = 0;
	window->columns = parentColumn - 2;
	window->rows = parentRows - 2;

	wmove(window->window, window->cursor_y, window->cursor_x);

	wtimeout(window->window, 100);
	mvwprintw(window->window, 1, 1, "%*.*s", 0, window->columns, window->command);
}