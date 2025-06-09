#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include "config.h"

#define log_info(...)  log_(__FILE__, __LINE__, __VA_ARGS__)

void init_log(FILE* log_file);

void log_(const char *file, int line, const char *fmt, ...);

#endif
