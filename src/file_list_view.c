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
    int MAXSIZE = 0xFFF;
    char proclnk[0xFFF];
    char filename[0xFFF];
    FILE *fp;
    int fno;
    ssize_t r;
    int line = 1;

    //wclear(file_list_view->window);

    FOR_EACH_IN_LIST(FILE*, el, list_of_files, {
        fno = fileno(el);
        
        sprintf(proclnk, "/proc/self/fd/%d", fno);
        r = readlink(proclnk, filename, MAXSIZE);
        if (r < 0)
        {
            mvwprintw(file_list_view->window, line++, 1, "%d: Couldnt read FD: %d", line - 1, fno);
        } else {
            filename[r] = '\0';
            mvwprintw(file_list_view->window, line++, 1, "%d: %s", line - 1, filename);
        }
    });
}