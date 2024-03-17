#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>

#include "command_window.h"
#include "log_window.h"
#include "config.h"
#include "log.h"
#include "message_window.h"

typedef int file_descriptor;

typedef struct Main_Panel {
    List* list_file_descriptors;
    Log_Window lw;
    Command_Window cw;
    Message_Window mw;
} Main_Panel;

int start_app(List* files);

#endif