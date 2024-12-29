#ifndef LOG_WINDOW_H_
#define LOG_WINDOW_H_

#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "logger.h"
#include "config.h"
#include "log_view.h"
#include "log_view_header.h"
#include "string_view.h"

typedef struct Viewport {
  int start;
  int end;
} Viewport;

typedef struct Log_Window {
  WINDOW* window;
  Viewport* viewport;
  int columns;
  int rows;
  int cursor_x;
  int cursor_y;
  int line_cursor;
  int screen_offset;
  List* lines_to_display;
  List* log_view_list;
  Log_View* lv_current;
  Log_View_Header* log_view_header;
  char* search_term;
} Log_Window;

Log_Window create_log_window(int parentRows, int parentColumn);

void process_log_window(Log_Window* log_window, char *line, int line_size);

void set_filter_log_window(Log_Window* window, char *command);

void resize_log_window(Log_Window* window, int parentRows, int parentColumn);

#endif
