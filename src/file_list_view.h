#ifndef __FILE_LIST_VIEW_H
#define __FILE_LIST_VIEW_H

#include <ncurses.h>
#include <stdbool.h>
#include "list.h"

typedef struct {

    int cols;
    int rows;
    bool isShowing;
    WINDOW* border_win;
    WINDOW* window;

} File_List_View;

File_List_View create_file_list_view(int parentRows, int parentColumn);

void show_file_list(File_List_View* file_list_view, List* list_of_files);

#endif //__FILE_LIST_VIEW_H