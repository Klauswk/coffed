#ifndef STRING_VIEW_H_
#define STRING_VIEW_H_

#include <stddef.h>
// printf macros for String_View
#define String_View_Fmt "%.*s"
#define String_View_Arg(sv) (int) (sv).size, (sv).text

typedef struct {
    char *text;
    size_t size;
} String_View;

String_View build_from_char(char* text, size_t size);

String_View get_next_line(String_View* sv, size_t line_size);

#endif