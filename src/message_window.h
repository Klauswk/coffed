#ifndef MESSAGE_WINDOW_H
#define MESSAGE_WINDOW_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum Message_Level {
  ML_ERROR = 10,
  ML_INFO = 11,
  ML_WARNING = 12
};

typedef struct Message_Window {
  WINDOW* window;
  int columns;
  int rows;
  bool is_showing;
} Message_Window;

Message_Window create_message_window(int parentRows, int parentColumn);

void show_message(Message_Window* message_window, const char* message, enum Message_Level level);

void clear_message(Message_Window* window);

#endif //MESSAGE_WINDOW_H
