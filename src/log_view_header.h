#ifndef LOG_VIEW_HEADER_H
#define LOG_VIEW_HEADER_H

#include <ncurses.h>
#include <string.h>
#include "config.h"
#include "list.h"
#include "logger.h"

typedef struct Log_View_Header_Item {
  char* header;
  int is_main;
  int header_size;
  WINDOW* header_window;
  WINDOW* header_border;
} Log_View_Header_Item;

typedef struct Log_View_Header {
  List* headers;
  Log_View_Header_Item* current;
} Log_View_Header;


Log_View_Header* create_log_view_header(int parentRows, int parentColumn, char* title);

void add_header_item(Log_View_Header* log_view_header, char* header);

void remove_header_at(Log_View_Header* log_view_header, Log_View_Header_Item* log_view_header_item);

void draw_header_view(Log_View_Header* header);

#endif
