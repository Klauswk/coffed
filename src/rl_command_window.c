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
#include <math.h>
#include <sys/param.h>

#define _XOPEN_SOURCE 700

int wcwidth(wchar_t c);

static int input;

static bool input_avail = false;

static Command_Window command_Window;

static int readline_input_avail(void)
{
    return input_avail;
}

static size_t strnwidth(const char *s, size_t n, size_t offset)
{
    mbstate_t shift_state;
    size_t wc_len = 0;
    size_t width = 0;

    // Start in the initial shift state
    memset(&shift_state, '\0', sizeof shift_state);

    for (size_t i = 0; i < n; i += wc_len)
    {
        wchar_t wc;
        wc_len = mbrtowc(&wc, s + i, MB_CUR_MAX, &shift_state);
        switch (wc_len)
        {
        case 0:
            return width;

        case (size_t)-1:
        case (size_t)-2:
            width += strnlen(s + i, n - i);
            return width;
        }

        if (wc == '\t')
        {
            width = ((width + offset + 8) & ~7) - offset;
        }
        else
        {
            width += iswcntrl(wc) ? 2 : MAX(0, wcwidth(wc));
        }
    }

    return width;
}

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
    mvwprintw(command_Window.window, 0, 0, "%s%s", rl_display_prompt, rl_line_buffer);
    if (cursor_col >= COLS)
        curs_set(0);
    else
    {
        wmove(command_Window.window, 0, cursor_col);
        curs_set(2);
    }
  
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
    if (*line)
    {
        add_history(line);
        size_t prompt_width = strwidth(rl_display_prompt, 0);
        size_t cursor_col = prompt_width +
                            strnwidth(rl_line_buffer, rl_point, prompt_width);
        size_t line_size = cursor_col - prompt_width;
				memset(command_Window.command, 0, command_Window.buffer_size); 
        strncpy(command_Window.command, line, line_size);
        command_Window.command[line_size + 1] = 0;
        command_Window.callback(command_Window.parent, command_Window.command);
        free(line);
    }
}

static void init_readline(void)
{
    if (rl_bind_key('\t', rl_insert))
        exit(1 && "Invalid key passed to rl_bind_key()");

    rl_catch_signals = 0;
    rl_catch_sigwinch = 0;
    rl_deprep_term_function = NULL;
    rl_prep_term_function = NULL;
    rl_change_environment = 0;
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
}

static void move_to_next_tab(Command_Window *window)
{
    window->callback(window->parent, ":MOVE_TO_NEXT_TAB\t");
}

static void move_to_previously_tab(Command_Window *window)
{
    window->callback(window->parent, ":MOVE_TO_PREVIOUSLY_TAB\t");
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

    wrefresh(window.window);
    wmove(window.window, window.cursor_y, window.cursor_x);

    wtimeout(window.window, 10);

    init_readline();

    return window;
}

bool handle_input_command_window(Command_Window *window)
{
    int input = wgetch(window->window);

    if (input != -1)
    {
        if (input == KEY_RESIZE)
        {
            resize_app_window(window);
            return false;
        }

        if (rl_line_buffer[0] == ':' || rl_line_buffer[0] == '/')
        {
            log_info("Typed the character with code %d and value %s \n", input, keyname(input));

            forward_to_readline(input);

            return true;
        }

        if (input == 'k')
        {
            log_info("Go up \n");
            window->callback(window->parent, ":GO_UP\t");

            // set_cursor_at_command_window(window);
            return false;
        }
        else if (input == 'j')
        {
            log_info("Go down \n");
            window->callback(window->parent, ":GO_DOWN\t");

            // set_cursor_at_command_window(window);
            return false;
        }
        else if (input == ('w' & 0x1F))
        {
            close_tab(window);
            return false;
        }
        else if (input == 'K')
        {
            log_info("Going to next tab \n");
            move_to_next_tab(window);
            return false;
        }
        else if (input == 'J')
        {
            log_info("Going to previously tab \n");
            move_to_previously_tab(window);
            return false;
        }
        else if (input == 9)
        {
            window->callback(window->parent, ":SHOW_FILE_LIST\t");
            return false;
        }

        if (input == ':' || input == '/')
        {
            forward_to_readline(input);
        }
    }
    return false;
}

void resize_command_window(Command_Window *window, int parentRows, int parentColumn)
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
