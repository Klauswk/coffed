#include "assert.h"

int errors = 0;
int total_tests = 0;
int total_errors = 0;

void _run_file(const int argc, char** argv, const char* file)
{
    //means we have no file selected for the tests
    if(argc == 1)
    {
        printf("\033[95mRunning %s\n\033[0m", file);
        return;
    }

    for(int i = 1; i < argc; i++)
    {
        if(strstr(file,argv[i]) != NULL)
        {
            printf("\033[95mRunning %s\n\033[0m", file);
            return;
        }
    }

    printf("\033[93mIgnoring file %s\n\033[0m", file);
    exit(0);
}

void _assert(const char *file, const char *func, int line, int expression, ...)
{

    char* message = NULL;
    
    va_list param;

    va_start(param, expression);

    message = va_arg(param, char*);
    
    va_end(param);

    if(!expression)
    {
        ++errors;
        ++total_errors;
        if(message)
        {
            fprintf(stderr, "\033[31m  * ❌ %s:%s:%d - Validation failed: %s \n\033[0m", file, func, line, message);
        }
        else
        {
            fprintf(stderr, "\033[31m * ❌ %s:%s:%d - Validation failed, no message provided \n\033[0m", file,func, line);
        }
    }
}