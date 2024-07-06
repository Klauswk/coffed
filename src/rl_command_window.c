#include "command_window.h"

#include <locale.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <time.h> 

#define _XOPEN_SOURCE 700

#define max(a, b)         \
  ({ typeof(a) _a = a;    \
     typeof(b) _b = b;    \
     _a > _b ? _a : _b; })

// Input character for readline
static int input;

// Used to signal "no more input" after feeding a character to readline
static bool input_avail = false;

static Command_Window command_Window;
// Not bothering with 'input_avail' and just returning 0 here seems to do the
// right thing too, but this might be safer across readline versions
static int readline_input_avail(void)
{
    return input_avail;
}

static size_t strnwidth(const char *s, size_t n, size_t offset)
{
    mbstate_t shift_state;
    wchar_t wc;
    size_t wc_len;
    size_t width = 0;

    // Start in the initial shift state
    memset(&shift_state, '\0', sizeof shift_state);

    for (size_t i = 0; i < n; i += wc_len) {
        // Extract the next multibyte character
        wc_len = mbrtowc(&wc, s + i, MB_CUR_MAX, &shift_state);
        switch (wc_len) {
        case 0:
            // Reached the end of the string
            goto done;

        case (size_t)-1: case (size_t)-2:
            // Failed to extract character. Guess that each character is one
            // byte/column wide each starting from the invalid character to
            // keep things simple.
            width += strnlen(s + i, n - i);
            goto done;
        }

        if (wc == '\t')
            width = ((width + offset + 8) & ~7) - offset;
        else
            // TODO: readline also outputs ~<letter> and the like for some
            // non-printable characters
            width += iswcntrl(wc) ? 2 : max(0, wcwidth(wc));
    }

done:
    return width;
}

// Like strnwidth, but calculates the width of the entire string
static size_t strwidth(const char *s, size_t offset)
{
    return strnwidth(s, SIZE_MAX, offset);
}

static void cmd_win_redisplay(bool for_resize)
{
    size_t prompt_width = strwidth(rl_display_prompt, 0);
    size_t cursor_col = prompt_width +
                        strnwidth(rl_line_buffer, rl_point, prompt_width);

    werase(command_Window.window);
    // This might write a string wider than the terminal currently, so don't
    // check for errors
    mvwprintw(command_Window.window, 0, 0, "%s%s", rl_display_prompt, rl_line_buffer);
    if (cursor_col >= COLS)
        // Hide the cursor if it lies outside the window. Otherwise it'll
        // appear on the very right.
        curs_set(0);
    else {
        wmove(command_Window.window, 0, cursor_col);
        curs_set(2);
    }
    // We batch window updates when resizing
    if (for_resize)
        wnoutrefresh(command_Window.window);
    else
        wrefresh(command_Window.window);
}

static int readline_getc(FILE *dummy)
{
    input_avail = false;
    return input;
}

static void forward_to_readline(int c)
{
    input = c;
    input_avail = true;
    rl_callback_read_char();
}

static void readline_redisplay(void)
{
    cmd_win_redisplay(false);
}

static void got_command(char *line)
{
    if (*line) {
        add_history(line);
        size_t prompt_width = strwidth(rl_display_prompt, 0);
        size_t cursor_col = prompt_width +
                    strnwidth(rl_line_buffer, rl_point, prompt_width);
        size_t line_size = cursor_col - prompt_width;
        memcpy(command_Window.command, line, line_size);
        command_Window.command[line_size + 1] = 0;
        command_Window.callback(command_Window.parent, command_Window.command);
    }
}

static void init_readline(void)
{
    // Disable completion. TODO: Is there a more robust way to do this?
    if (rl_bind_key('\t', rl_insert))
        exit(1 && "Invalid key passed to rl_bind_key()");

    // Let ncurses do all terminal and signal handling
    rl_catch_signals = 0;
    rl_catch_sigwinch = 0;
    rl_deprep_term_function = NULL;
    rl_prep_term_function = NULL;

    // Prevent readline from setting the LINES and COLUMNS environment
    // variables, which override dynamic size adjustments in ncurses. When
    // using the alternate readline interface (as we do here), LINES and
    // COLUMNS are not updated if the terminal is resized between two calls to
    // rl_callback_read_char() (which is almost always the case).
    rl_change_environment = 0;

    // Handle input by manually feeding characters to readline
    rl_getc_function = readline_getc;
    rl_input_available_hook = readline_input_avail;
    rl_redisplay_function = readline_redisplay;

    rl_callback_handler_install("> ", got_command);
}

static void deinit_readline(void)
{
    rl_callback_handler_remove();
}

static void resize_app_window(Command_Window *window)
{
	window->callback(window->parent, ":RESIZE_WINDOW\t");
}

static void close_tab(Command_Window *window)
{

	log_info("Closing current tab \n");

	window->callback(window->parent, ":CLOSE_CURRENT_TAB\t");

	// set_cursor_at_command_window(window);
}

static void move_to_next_tab(Command_Window *window)
{
	window->callback(window->parent, ":MOVE_TO_NEXT_TAB\t");

	//set_cursor_at_command_window(window);
}

static void move_to_previously_tab(Command_Window *window)
{
	window->callback(window->parent, ":MOVE_TO_PREVIOUSLY_TAB\t");

	//set_cursor_at_command_window(window);
}

Command_Window create_command_window(int parentRows, int parentColumn, int buffer_size, void *parent, Submit_Filter_Callback callback)
{
	Command_Window window = {0};
	WINDOW *newWindow = newwin(3, parentColumn, parentRows - 2, 0);
	keypad(newWindow, false);
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

    command_Window = window;

	//clear_input(window.command, window.buffer_size);
	wrefresh(window.window);
	wmove(window.window, window.cursor_y, window.cursor_x);

	wtimeout(window.window, 10);

    init_readline();

	return window;
}

/*#define manual_buffer_size 6

char manual_keypad_buffer[manual_buffer_size];
int place = 0;

clock_t before = 0;
int msec = 0, trigger = 5; 
int start_buffering = 0;

static void test(char buffer_input) {

    if(before == 0) {
        before = clock();
    }

    clock_t difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;

    if(msec > trigger) {
        before = 0;
        msec = 0;
        place = 0;
        start_buffering = 0;

        int result = 0;

        for(int i = 0; i < manual_buffer_size; i+=2) {
            result += (manual_keypad_buffer[i] & manual_keypad_buffer[i + 1]);
        }
        log_info("%s %s %s \n", keyname(manual_keypad_buffer[0] & manual_keypad_buffer[1]),
        keyname(manual_keypad_buffer[2] & manual_keypad_buffer[3]),
        keyname(manual_keypad_buffer[4] & manual_keypad_buffer[5]));
        
        log_info("Buffered result %d and value %s \n", result, keyname(result));

        for(int i = 0; i < manual_buffer_size; i++) {
            manual_keypad_buffer[i] = 0;
        }
    } else {
        if(place >= manual_buffer_size) {
            place = 0;
        }
        if(buffer_input == 27 && place == 0) {
            //manual_keypad_buffer[place++] = buffer_input;
            start_buffering = 1;
        } 
        
        if (start_buffering) {
            manual_keypad_buffer[place++] = buffer_input;
        }
    }
}*/

bool handle_input_command_window(Command_Window *window)
{
	int input = wgetch(window->window);
    
	if (input != -1)
	{
        //test(input);
		if (input == ('k' & 0x1F))
		{
            log_info("Go up\n");
			window->callback(window->parent, ":GO_UP\t");

			//set_cursor_at_command_window(window);
			return false;
		}
		else if (input == ('j' & 0x1F))
		{
            log_info("Go down\n");
			window->callback(window->parent, ":GO_DOWN\t");

			//set_cursor_at_command_window(window);
			return false;
		}
		else if (input == KEY_RESIZE)
		{
			resize_app_window(window);
			return false;
		} 
		else if (input == ('w' & 0x1F))
		{
			close_tab(window);
            return false;
		}
		else if (input == ('n' & 0x1F))
		{
			log_info("Going to next tab \n");
			move_to_next_tab(window);
			return false;
		}
		else if (input == ('p' & 0x1F))
		{
			log_info("Going to previously tab \n");
			move_to_previously_tab(window);
			return false;
		} else if(input == 9) {
			window->callback(window->parent, ":SHOW_FILE_LIST\t");
			return false;
		}

        log_info("Typed the character with code %d and value %s \n", input, keyname(input));
		
        forward_to_readline(input);
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

    command_Window = *window;

	// wmove(window->window, window->cursor_y, window->cursor_x);

	wtimeout(window->window, 10);
	// mvwprintw(window->window, 1, 1, "%*.*s", 0, window->columns, window->command);
}