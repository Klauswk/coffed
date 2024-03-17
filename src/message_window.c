#include "message_window.h"

static int WINDOW_HEIGHT_OFFSET = 3;
static int WINDOW_OFFSET = 2;

Message_Window create_message_window(int parentRows, int parentColumn)
{
    Message_Window window = {0};
    WINDOW *newWindow = newwin(1, parentColumn, parentRows - 3, 0);
    window.window = newWindow;
    window.is_showing = false;

    getmaxyx(newWindow, window.rows, window.columns);

    return window;
}

void show_message(Message_Window* window, const char* message) 
{
	window->is_showing = true;
    
    wbkgd(window->window, COLOR_PAIR(3));
    mvwprintw(window->window, 0, 1, "%*.*s", 0, window->columns, message);
    wrefresh(window->window);
}

void clear_message(Message_Window* window) {
    window->is_showing = false;
    wbkgd(window->window, COLOR_PAIR(1));
    wclear(window->window);
    wrefresh(window->window);
}