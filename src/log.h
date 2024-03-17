#ifndef _LOG_H
#define _LOG_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "logger.h"

typedef struct {
    char* line;
    size_t count;
    size_t capacity;
} Log;

bool get_next_log(Log* log, FILE* file);

#endif