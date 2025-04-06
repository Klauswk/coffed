#include "log_window.h"
#include <assert.h>

static void free_node(Node *node)
{
  free(node->value);
  free(node);
}

static void free_sv_node(Node *node)
{
  String_View *sv = node->value;
  free(sv->text);
  free(node);
}

Log_Window create_log_window(int parentRows, int parentColumn)
{
  Log_Window window = {0};
  WINDOW *newWindow = newwin(parentRows - 3, parentColumn, 1, 0);
  init_pair(5, COLOR_CYAN, COLOR_BLACK); 
  init_pair(6, COLOR_CYAN, COLOR_YELLOW); 
  window.line_cursor = -1;
  window.window = newWindow;
  window.screen_offset = 0;
  window.viewport = malloc(sizeof(Viewport));
  window.viewport->start = 0;
  window.viewport->end = parentRows;
  window.search_term = NULL;

  window.lines_to_display = malloc(sizeof(List));
  init_list(window.lines_to_display, MAX_LIST_SIZE, free_sv_node);

  window.log_view_list = malloc(sizeof(List));
  init_list(window.log_view_list, MAX_LIST_SIZE, free_node);

  Log_View *lv_main = create_log_view(parentRows - 1, parentColumn);
  lv_main->is_main = 1;

  add_to_list(window.log_view_list, lv_main);

  window.lv_current = window.log_view_list->head->value;
  window.log_view_header = create_log_view_header(parentRows, parentColumn, "Main");

  getmaxyx(newWindow, window.rows, window.columns);

  wrefresh(window.window);

  draw_header_view(window.log_view_header);

  return window;
}

static void clear_log_window(Log_Window *window)
{
  window->line_cursor = -1;
}

static void increase_line_counter_by(Log_Window *window, int value)
{
  window->line_cursor += value;
}

static void add_line_to_log(Log_Window *window, String_View sv)
{

  for (size_t i = 0; i < window->columns; i++)
  {
    wattrset(window->window, 0);
    mvwprintw(window->window, window->line_cursor, i, " ");
  }

  size_t acc = 0;

  if (window->search_term != NULL) {
    size_t term_size = strlen(window->search_term);

    if (term_size > 0) {
      char* result = find_text_in_sv(sv, window->search_term);

      if (result != NULL) {

        do {
          size_t result_size = result - sv.text;

          String_View sv1 = chop_by_size(&sv, result_size);
          mvwprintw(window->window, window->line_cursor, 0, String_View_Fmt, String_View_Arg(sv1));
          acc = acc + result_size;

          String_View sv2 = chop_by_size(&sv,term_size); 
          if (window->marked_term_in_line.size > 0 && (sv2.text == window->marked_term_in_line.text)) {
            wattron(window->window, COLOR_PAIR(6));
            mvwprintw(window->window, window->line_cursor, acc, String_View_Fmt, String_View_Arg(sv2));
            wattroff(window->window, COLOR_PAIR(6));
            acc = acc + sv2.size;
          } else {
            wattron(window->window, COLOR_PAIR(5));
            mvwprintw(window->window, window->line_cursor, acc, String_View_Fmt, String_View_Arg(sv2));
            wattroff(window->window, COLOR_PAIR(5));
            acc = acc + sv2.size;
          }
          result = find_text_in_sv(sv, window->search_term);
        } while (result != NULL);

      }
    }
  }
  mvwprintw(window->window, window->line_cursor, acc, String_View_Fmt, String_View_Arg(sv));
}

static void process_list_to_lines(Log_Window *window, List *list)
{

  FOR_EACH_IN_LIST(char *, line, list, {
      String_View sv = build_from_char(line, strlen(line) + 1);
      String_View next_line = chop_by_size(&sv, window->columns);
      int number_of_lines = 0;
      while (next_line.size > 0)
      {
      String_View *new_line = malloc(sizeof(new_line));

      new_line->size = next_line.size;
      new_line->text = malloc(sizeof(char) * next_line.size);
      strncpy(new_line->text, next_line.text, new_line->size);

      add_to_list(window->lines_to_display, new_line);

      next_line = chop_by_size(&sv, window->columns);
      number_of_lines++;
      }
      });
}

static void redraw_log(Log_Window *window, Viewport *vp, int screen_offset)
{
  clear_log_window(window);

  int start = vp->start - screen_offset;
  int end = vp->end - screen_offset;

  if (start < 0)
  {
    start = 0;
  }

  if (end > window->lines_to_display->size)
  {
    end = window->lines_to_display->size;
  }

  for (int i = start; i < end; i++)
  {
    increase_line_counter_by(window, 1);
    String_View *sv = list_get_value_at(window->lines_to_display, i);
    add_line_to_log(window, *sv);
  }
}

static List *get_current_list(Log_Window *window)
{
  return window->lv_current->content_list;
}

static Log_View *get_main_view(Log_Window *window)
{
  Node *n = window->log_view_list->head;

  do
  {
    Log_View *lv = n->value;

    if (lv->is_main)
    {
      return lv;
    }
    n = n->next;
  } while (n);

  return window->log_view_list->head->value;
}

static void scroll_up(Log_Window *window)
{
  if ((window->lines_to_display->size - window->screen_offset) > window->rows)
  {
    window->screen_offset++;
    redraw_log(window, window->viewport, window->screen_offset);
  }
}

static void page_up(Log_Window *window)
{
  int page_size = window->rows;

  while(page_size > 0 && (window->lines_to_display->size - window->screen_offset) > window->rows) {
    page_size--;
    window->screen_offset++;
  }
  redraw_log(window, window->viewport, window->screen_offset);
}

static void page_down(Log_Window *window)
{
  if (window->screen_offset > 0) {
    int page_size = window->rows;

    while(page_size > 0 && window->screen_offset > 0) {
      page_size--;
      window->screen_offset--;
    }
    redraw_log(window, window->viewport, window->screen_offset);
  }
}

static void scroll_down(Log_Window *window)
{
  if (window->screen_offset > 0)
  {
    window->screen_offset--;
  }
  else if (window->screen_offset == 0)
  {
    window->viewport->start = window->lines_to_display->size - window->rows;
    window->viewport->end = window->lines_to_display->size;
  }
  redraw_log(window, window->viewport, window->screen_offset);
}

static void close_tab(Log_Window *window)
{
  if (!window->log_view_header->current->is_main)
  {
    // Remove the header
    remove_header_at(window->log_view_header, window->log_view_header->current);
    window->log_view_header->current = window->log_view_header->headers->tail->value;

    // Remove the log view list
    remove_value_from_list(window->log_view_list, window->lv_current);
    window->lv_current = window->log_view_list->tail->value;
    draw_header_view(window->log_view_header);

    List *cl = get_current_list(window);
    clear_list(window->lines_to_display);
    process_list_to_lines(window, cl);
    wclear(window->window);

    window->screen_offset = 0;
    window->viewport->start = window->lines_to_display->size - window->rows;
    window->viewport->end = window->lines_to_display->size;
    redraw_log(window, window->viewport, window->screen_offset);
  }
}

static void move_to_next_tab(Log_Window *window)
{
  Log_View_Header *lvh = window->log_view_header;

  if (lvh->headers->size == 1)
  {
    return;
  }

  Node *n = find_node_on_value(lvh->headers, lvh->current);

  if (n->next != NULL)
  {
    lvh->current = n->next->value;
  }
  else
  {
    lvh->current = lvh->headers->head->value;
  }

  n = find_node_on_value(window->log_view_list, window->lv_current);

  if (n->next != NULL)
  {
    window->lv_current = n->next->value;
  }
  else
  {
    window->lv_current = window->log_view_list->head->value;
  }

  List *cl = get_current_list(window);
  draw_header_view(window->log_view_header);

  clear_list(window->lines_to_display);
  process_list_to_lines(window, cl);
  wclear(window->window);

  window->screen_offset = 0;
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;
  redraw_log(window, window->viewport, window->screen_offset);
}

static void move_to_previously_tab(Log_Window *window)
{
  Log_View_Header *lvh = window->log_view_header;

  if (lvh->headers->size == 1)
  {
    return;
  }

  Node *n = find_node_on_value(lvh->headers, lvh->current);

  if (n->previous != NULL)
  {
    lvh->current = n->previous->value;
  }
  else
  {
    lvh->current = lvh->headers->tail->value;
  }

  n = find_node_on_value(window->log_view_list, window->lv_current);

  if (n->previous != NULL)
  {
    window->lv_current = n->previous->value;
  }
  else
  {
    window->lv_current = window->log_view_list->tail->value;
  }

  List *cl = get_current_list(window);
  draw_header_view(window->log_view_header);

  clear_list(window->lines_to_display);
  process_list_to_lines(window, cl);
  wclear(window->window);

  window->screen_offset = 0;
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;
  redraw_log(window, window->viewport, window->screen_offset);
}

static void move_to_top(Log_Window *window)
{
  window->screen_offset = window->lines_to_display->size - window->rows;
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;
  redraw_log(window, window->viewport, window->screen_offset);
}

static void move_to_bottom(Log_Window *window)
{
  window->screen_offset = 0;
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;
  redraw_log(window, window->viewport, window->screen_offset);
}

static void go_to_next_occourence(Log_Window *window) {
  if (window->search_term && *window->search_term) {
    int found_index = 0;

    Node *n = window->lines_to_display->head;                              

    // First we need to find the current occourence
    if (window->marked_term_in_line.text != NULL) {
      for (size_t index = 0; index < window->lines_to_display->size; index++) 
      {                                                   
        String_View* e = n->value;                        
        char* substring = strstr(e->text, window->search_term); 
        if (substring != NULL && window->marked_term_in_line.text == substring) {
          found_index = index;
          break;
        }
        n = n->next;                                    
      }
    }

    for (size_t index = found_index; index < window->lines_to_display->size; index++) 
    {                                                   
      String_View* e = n->value;                        
      char* substring = strstr(e->text, window->search_term); 
      if (substring != NULL && window->marked_term_in_line.text != substring) {
        found_index = 1;
        window->marked_term_in_line.text = substring;
        window->marked_term_in_line.size = strlen(window->search_term);
        break;
      }
      n = n->next;                                    
    }

    if (!found_index) {
      window->marked_term_in_line.text = NULL;
      window->marked_term_in_line.size = 0;
    }
  }
  redraw_log(window, window->viewport, window->screen_offset);
}

static void go_to_previous_occourence(Log_Window *window) {
  if (window->search_term && *window->search_term) {
    int found_index = 0;

    Node *n = window->lines_to_display->head;                              
    // First we need to find the current occourence
    if (window->marked_term_in_line.text != NULL) {
      for (size_t index = 0; index < window->lines_to_display->size - 1; index++) 
      {                                                   
        String_View* e = n->value;                        
        char* substring = strstr(e->text, window->search_term); 
        if (substring != NULL && window->marked_term_in_line.text == substring) {
          found_index = index;
          break;
        }
        n = n->next;
        assert(n->value);
      }
    }

    if (found_index == 0) {
      found_index = window->lines_to_display->size - 1;
      n = window->lines_to_display->tail;
      assert(n->value);
    }

    for (size_t index = found_index; index > 0; index--) 
    {
      assert(n->value);
      String_View* e = n->value;                        
      char* substring = strstr(e->text, window->search_term); 
      if (substring != NULL && window->marked_term_in_line.text != substring) {
        found_index = 1;
        window->marked_term_in_line.text = substring;
        window->marked_term_in_line.size = strlen(window->search_term);
        break;
      }
      n = n->previous;                                    
    }

    if (!found_index) {
      window->marked_term_in_line.text = NULL;
      window->marked_term_in_line.size = 0;
    }
  }

  redraw_log(window, window->viewport, window->screen_offset);
}

static void process_filter(Log_Window *window, char *command)
{
  window->screen_offset = 0;
  wclear(window->window);

  Log_View *lv_filter = create_log_view(window->columns, window->rows);

  add_to_list(window->log_view_list, lv_filter);

  memcpy(lv_filter->filter_command, ++command, BUFFER_SIZE);

  add_header_item(window->log_view_header, lv_filter->filter_command);

  draw_header_view(window->log_view_header);

  window->lv_current = lv_filter;
  log_info("Also filtering\n");

  Log_View *main_view = get_main_view(window);

  Node *n = main_view->content_list->head;

  clear_log_window(window);

  clear_list(window->lines_to_display);

  if (main_view != NULL)
  {
    for (int i = 0; i < main_view->content_list->size; i++)
    {
      if (strstr(n->value, lv_filter->filter_command) != NULL)
      {

        size_t size = sizeof(char) * (strnlen(n->value, BUFFER_SIZE - 1)) + 1;
        void *value = malloc(size);
        memcpy(value, n->value, size);
        add_to_list(lv_filter->content_list, value);
      }

      n = n->next;
    }

    process_list_to_lines(window, lv_filter->content_list);
  }

  window->screen_offset = 0;
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;
  redraw_log(window, window->viewport, window->screen_offset);
}

static void dump_to_file(Log_Window *window, char *file)
{
  if (!window->lv_current->is_main)
  {

    FILE *dump_file = fopen(file, "a");

    if (dump_file == NULL)
    {
      printf("It was not possible to open the file %s \n", (char *)dump_file);
      log_info("It was not possible to open the file %s \n", (char *)dump_file);
      return;
    }

    FOR_EACH_IN_LIST(char *, line, window->lv_current->content_list, {
        fprintf(dump_file, "%s\n", line);
        });

    fclose(dump_file);
  }
}

void resize_log_window(Log_Window *window, int parentRows, int parentColumn)
{
  delwin(window->window);

  WINDOW *newWindow = newwin(parentRows - 3, parentColumn, 1, 0);

  getmaxyx(window->window, window->rows, window->columns);

  window->window = newWindow;

  List *cl = get_current_list(window);

  clear_list(window->lines_to_display);
  process_list_to_lines(window, cl);
  window->viewport->start = window->lines_to_display->size - window->rows;
  window->viewport->end = window->lines_to_display->size;

  redraw_log(window, window->viewport, window->screen_offset);
}

void process_log_window(Log_Window *window, char *line, int line_size)
{
  Node *n = window->log_view_list->head;

  int added = 0;

  while (n)
  {
    Log_View *lv = n->value;

    if (lv == window->lv_current)
    {
      added = add_to_log_view(lv, line, line_size);
    }
    else
    {
      add_to_log_view(lv, line, line_size);
    }

    n = n->next;
  }

  if (added)
  {
    String_View sv = build_from_char(line, line_size);
    String_View next_line = chop_by_size(&sv, window->columns);
    int number_of_lines = 0;
    while (next_line.size > 0)
    {
      String_View *new_line = malloc(sizeof(new_line));

      new_line->size = next_line.size;
      new_line->text = malloc(sizeof(char) * next_line.size);
      strncpy(new_line->text, next_line.text, new_line->size);

      add_to_list(window->lines_to_display, new_line);

      next_line = chop_by_size(&sv, window->columns);
      number_of_lines++;
    }

    if (window->screen_offset > 0) {
      window->screen_offset += number_of_lines;  
    }
    if (window->lines_to_display->size >= (window->rows))
    {
      window->viewport->start = window->lines_to_display->size - window->rows;
      window->viewport->end = window->lines_to_display->size;
      redraw_log(window, window->viewport, window->screen_offset);
    }
    else
    {
      for (size_t i = window->lines_to_display->size - number_of_lines; i < window->lines_to_display->size; i++)
      {
        increase_line_counter_by(window, 1);
        String_View *list_sv = list_get_value_at(window->lines_to_display, i);
        add_line_to_log(window, *list_sv);
      }
    }
  }
}

void set_filter_log_window(Log_Window *window, char *command)
{
  if (command != NULL)
  {
    if (command[0] == ':')
    {
      if (strstr(command, ":GO_UP\t") != NULL)
      {
        scroll_up(window);
        return;
      }
      else if (strstr(command, ":GO_DOWN\t") != NULL)
      {
        scroll_down(window);
        return;
      }
      else if (strstr(command, ":PAGE_DOWN\t") != NULL)
      {
        page_down(window);
        return;
      }
      else if (strstr(command, ":PAGE_UP\t") != NULL)
      {
        page_up(window);
        return;
      }
      else if (strstr(command, ":CLOSE_CURRENT_TAB\t") != NULL)
      {
        close_tab(window);
        return;
      }
      else if (strstr(command, ":MOVE_TO_NEXT_TAB\t") != NULL)
      {
        move_to_next_tab(window);
        return;
      }
      else if (strstr(command, ":MOVE_TO_PREVIOUSLY_TAB\t") != NULL)
      {
        move_to_previously_tab(window);
        return;
      }
      else if (strstr(command, ":MOVE_TO_TOP\t") != NULL)
      {
        move_to_top(window);
        return;
      }
      else if (strstr(command, ":MOVE_TO_BOTTOM\t") != NULL)
      {
        move_to_bottom(window);
        return;
      }
      else if (strstr(command, ":dump ") != NULL)
      {
        char file_path[BUFFER_SIZE] = "";
        memcpy(file_path, command + 6, BUFFER_SIZE - 6);
        dump_to_file(window, file_path);
        return;
      } else if (strstr(command, ":NEXT_OCCURRENCE\t") != NULL && window->search_term != NULL) {
        go_to_next_occourence(window);   
      } else if (strstr(command, ":PREV_OCCURRENCE\t") != NULL && window->search_term != NULL) {
        go_to_previous_occourence(window); 
      }
    }
    else if (command[0] == '&')
    {
      if (window->search_term != NULL) {
        free(window->search_term);
        window->search_term = NULL;
      }
      window->marked_term_in_line.text = NULL;
      window->marked_term_in_line.size = 0;
      process_filter(window, command);
    }
    else if (command[0] == '/')
    {
      if (window->search_term != NULL) {
        free(window->search_term);
        window->search_term = NULL;
      }
      if (strlen(command) > 1) {
        command++;
        window->search_term = calloc(strlen(command), sizeof(char));
        memcpy(window->search_term, command, strlen(command));
        window->marked_term_in_line.text = NULL;
        window->marked_term_in_line.size = 0;
        log_info("New search term is %s\n", window->search_term);

        go_to_previous_occourence(window); 
      }
      window->viewport->start = window->lines_to_display->size - window->rows;
      window->viewport->end = window->lines_to_display->size;
      redraw_log(window, window->viewport, window->screen_offset);
    }
    else
    {
      log_info("Couldnt process the command: %s\n", command);
    }
  }
}

