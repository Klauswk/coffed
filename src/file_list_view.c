#include "file_list_view.h"
#include <unistd.h>
#include <stdlib.h>

File_List_View create_file_list_view(int parentRows, int parentColumn) 
{
	File_List_View flv = {0};
	WINDOW *newWindow = newwin(parentRows/2, parentColumn/2, parentRows/4, parentColumn/4);
    box(newWindow, 0, 0);
    flv.window = newWindow;
    flv.isShowing = false;

    return flv;
}

void show_file_list(File_List_View* file_list_view, List* list_of_files) 
{
    int line = 1;

    FOR_EACH_IN_LIST(FILE*, el, list_of_files, {
        char proc_link_path[4096];
        char filename[4096];
        int file_descriptor = fileno(el);
        
        sprintf(proc_link_path, "/proc/self/fd/%d", file_descriptor);
        ssize_t byte_count = readlink(proc_link_path, filename, 4096);
        if (byte_count < 0)
        {
            mvwprintw(file_list_view->window, line++, 1, "%d: Couldnt read FD: %d", line - 1, file_descriptor);
        } else {
            filename[byte_count] = '\0';
            mvwprintw(file_list_view->window, line++, 1, "%d: %s", line - 1, filename);
        }
    });
}