#ifndef MAIN_PANEL_H
#define MAIN_PANEL_H

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <panel.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "command_window.h"
#include "log_window.h"
#include "config.h"
#include "log.h"
#include "message_window.h"
#include "file_list_view.h"
#include "formater.h"

typedef int file_descriptor;

typedef struct Main_Panel {
    List* list_file_descriptors;
    Node *current_file;
    Log_Window lw;
    Command_Window cw;
    Message_Window mw;
    File_List_View lfv;
    PANEL* panels[2];
    PANEL* top;
} Main_Panel;


int start_app(List* files);

#endif
