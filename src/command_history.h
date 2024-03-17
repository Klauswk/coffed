#ifndef COMMAND_HISTORY_H
#define COMMAND_HISTORY_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "logger.h"

typedef struct {
    FILE* file_location;
    char** list_of_commands;
    long current_position;
    size_t count;
    size_t capacity;
} Command_History;

Command_History init_command_history(const char* command_file);

void add_to_command_history(Command_History* command_history, const char* command);

const char* get_previous_command(Command_History* command_history);

const char* get_next_command(Command_History* command_history);

#endif