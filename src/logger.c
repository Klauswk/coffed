#include "logger.h"

static FILE* log_file;

static long long counter = 0;

void init_log(FILE* log) {
  log_file = log;
  setlinebuf(log_file);
}

void log_(const char *file, int line, const char *fmt, ...) {
#ifdef DEBUG 
  counter++;
  fprintf(log_file, "%lld: %s:%d - ",counter, file, line);
  va_list args;
  va_start( args, fmt);
  vfprintf (log_file, fmt, args);
  va_end (args);
#endif
}
