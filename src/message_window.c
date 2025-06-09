#include "message_window.h"

static int WINDOW_HEIGHT_OFFSET = 3;
static int WINDOW_OFFSET = 2;

Message_Window create_message_window(int parentRows, int parentColumn)
{
  Message_Window window = {0};
  WINDOW *newWindow = newwin(1, parentColumn, parentRows - 2, 0);
  window.window = newWindow;
  window.is_showing = false;

  getmaxyx(newWindow, window.rows, window.columns);

  if (has_colors())
  {
    init_pair(ML_ERROR, COLOR_WHITE, COLOR_RED);
    init_pair(ML_INFO, COLOR_WHITE, COLOR_GREEN);
    init_pair(ML_WARNING, COLOR_WHITE, COLOR_YELLOW);
  }

  return window;
}

void show_message(Message_Window* window, const char* message, enum Message_Level level) 
{
  window->is_showing = true;

  wbkgd(window->window, COLOR_PAIR(level));
  mvwprintw(window->window, 0, 1, "%*.*s", 0, window->columns, message);
  wrefresh(window->window);
}

void clear_message(Message_Window* window) {
  window->is_showing = false;
  wbkgd(window->window, COLOR_PAIR(1));
  wclear(window->window);
  wrefresh(window->window);
}
