#ifndef COMMAND_WINDOW_H_
#define COMMAND_WINDOW_H_

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "logger.h"
#include "command_history.h"

typedef void (*Submit_Filter_Callback)(void* mp, char* filter_command);

typedef struct Command_Window {
	void* parent;
	WINDOW* window;
	int columns;
	int rows;
	int cursor_x;
	int cursor_y;
    int input_position;
    int buffer_size;
    char* command;
	Submit_Filter_Callback callback;
	Command_History command_history;
} Command_Window;

Command_Window create_command_window(int parentRows, int parentColumn, int buffer_size, void* parent, Submit_Filter_Callback callback);

bool handle_input_command_window(Command_Window* window);

void resize_command_window(Command_Window* window, int parentRows, int parentColumn);

#endif