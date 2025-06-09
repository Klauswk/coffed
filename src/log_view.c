#include "log_view.h"

static void free_node(Node* node)
{
  free(node->value);
  free(node);
}

static int should_add(Log_View* log_view, char* line)
{
  return log_view->filter_command[0] == 0 ? 1 : strstr(line, log_view->filter_command) != NULL;
}

Log_View* create_log_view(int parentRows, int parentColumn)
{
  Log_View* lv = malloc(sizeof(Log_View));
  lv->is_main = 0;
  lv->content_list = malloc(sizeof(List));
  init_list(lv->content_list, MAX_LIST_SIZE, free_node);
  lv->filter_command = malloc(sizeof(char) * BUFFER_SIZE);
  memset(lv->filter_command, 0, BUFFER_SIZE);

  return lv;
}

int add_to_log_view(Log_View* log_view, char* line, int line_size)
{
  int result = 0;

  if(should_add(log_view, line))
  {
    char *newLine = malloc(sizeof(char) * (line_size + 1));
    memcpy(newLine, line, line_size);
    newLine[line_size] = '\0';

    add_to_list(log_view->content_list, newLine);
    result = 1;
  } 

  return result;
}
