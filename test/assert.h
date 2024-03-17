#ifndef __CUSTOM_ASSERT_LIB
#define __CUSTOM_ASSERT_LIB

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern int errors;
extern int total_tests;
extern int total_errors;

#define assert(...)  _assert(__FILE__,__func__, __LINE__, __VA_ARGS__, NULL)

#define RUN_FILE(argc, argv, body) \
    do { \
    _run_file(argc, argv, __FILE__); \
    body; \
    printf("Result: %d / %d \n", total_tests - total_errors, total_tests); \
} while(0) \

#define RUN_TEST(func) \
    printf("\033[94mRunning test: [%s]\n\033[0m", #func);\
    func(); \
    if(errors == 0) \
        printf(" * \033[32m✅ %s OK\n\033[0m", #func); \
    else \
        errors = 0; \
    total_tests++    \

void _assert(const char *file, const char *func, int line, int expression, ...);

void _run_file(const int argc, char** argv, const char* file);

#endif