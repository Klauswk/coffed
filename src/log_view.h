#ifndef LOG_VIEW_H
#define LOG_VIEW_H

#include "config.h"
#include "list.h"

typedef struct Log_View {
	char* filter_command;
	List* content_list;
	int is_main;
} Log_View;

int add_to_log_view(Log_View* log_view, char* line, int line_size);

Log_View* create_log_view(int parentRows, int parentColumn);

#endif