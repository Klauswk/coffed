#include "log_view_header.h"

static void free_node(Node* node)
{
    Log_View_Header_Item* window = node->value;

    free(window->header);
    wclear(window->header_border);
    wrefresh(window->header_border);
    delwin(window->header_window);
    delwin(window->header_border);
    free(node->value);
	free(node);
}

static int limit_number_of_caracters(int size) {
    return size > HEADER_MAX_SIZE? HEADER_MAX_SIZE: size;
}

Log_View_Header* create_log_view_header(int parentRows, int parentColumn, char* title)
{
    Log_View_Header* window = malloc(sizeof(Log_View_Header));

    window->headers = malloc(sizeof(List));
    init_list(window->headers, MAX_LIST_SIZE, free_node);

    add_header_item(window, title);
    
    Log_View_Header_Item* lv_main = window->headers->head->value;

    lv_main->is_main = 1;
    window->current = lv_main;
    
    return window;
}

void add_header_item(Log_View_Header* log_view_header, char* command)
{
    size_t command_size = strlen(command) + 1;
    char *header = malloc(sizeof(char) * command_size);
    memcpy(header, command, command_size);

    int numberOfLines = 3;
    int numberOfColumns = limit_number_of_caracters(strlen(header) + 1); //+ 1 for extra *
    int borderSideSize = 2; // 1 for each side of the border 
    int beginY = 1; // the border consume the first column
    int beginX = 1; // the border consume the first row
    int internalWindowLines = 1;
    
    int commulative_X = 0;

    Node* n = log_view_header->headers->head;

    while (n != NULL)
    {
        Log_View_Header_Item* lvhi = n->value;
        commulative_X += lvhi->header_size;
        commulative_X += 2;
        n = n->next;
    }
    
    Log_View_Header_Item* window = malloc(sizeof(Log_View_Header_Item));
    window->header_border = newwin(numberOfLines, numberOfColumns + borderSideSize, 0, commulative_X);
    window->header_window = derwin(window->header_border, internalWindowLines, numberOfColumns, beginY, beginX);
    window->is_main = 0;
    window->header = header;
    window->header_size = numberOfColumns;
    log_view_header->current = window;
    add_to_list(log_view_header->headers, window); 
}

static void clear_header(Log_View_Header* log_view_header)
{
    Node* n = log_view_header->headers->head;

    while (n != NULL)
    {
        Log_View_Header_Item* lvhie = n->value;
        wclear(lvhie->header_border);
        wrefresh(lvhie->header_border);

        n = n->next;
    }
}

void remove_header_at(Log_View_Header* log_view_header, Log_View_Header_Item* lvhi)
{
    if(!lvhi->is_main)
    {
        clear_header(log_view_header);
        remove_value_from_list(log_view_header->headers, lvhi);

        Node* n = log_view_header->headers->head->next;

        Log_View_Header_Item* header = log_view_header->headers->head->value;

        int commulative_X = 0 + header->header_size + 2;
        while (n != NULL)
        {

            Log_View_Header_Item* lvhie = n->value;
            mvwin(lvhie->header_border, 0, commulative_X);
            delwin(lvhie->header_window);

            int numberOfColumns = limit_number_of_caracters(strlen(lvhie->header) + 1); //+ 1 for extra *
            int beginY = 1; // the border consume the first column
            int beginX = 1; // the border consume the first row
            int internalWindowLines = 1;

            lvhie->header_window = derwin(lvhie->header_border, internalWindowLines, numberOfColumns, beginY, beginX);
            commulative_X += lvhie->header_size;
            commulative_X += 2;

            n = n->next;
        }

    }

}

void draw(Log_View_Header* window)
{
    List* headers = window->headers;

	Node* n = headers->head;

    int i = 1;

	do
	{
        Log_View_Header_Item* lvhi = n->value;
        box(lvhi->header_border, 0, 0);
        if(lvhi == window->current)
        {
            wbkgd(lvhi->header_window, COLOR_PAIR(2));
            wrefresh(lvhi->header_border);
            mvwprintw(lvhi->header_window, 0, 0, "*%*.*s", 0, 10, lvhi->header);
            wrefresh(lvhi->header_window);
        }
        else
        {
            wbkgd(lvhi->header_window, COLOR_PAIR(1));
            wrefresh(lvhi->header_border);
            mvwprintw(lvhi->header_window, 0, 0, " %*.*s", 0, 10, lvhi->header);
            wrefresh(lvhi->header_window);
        }
        
		n = n->next;
	} while (n != NULL);

}
