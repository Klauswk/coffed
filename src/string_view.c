#include "string_view.h"

String_View build_from_char(char* text, size_t size) {
    
    String_View sv;
    sv.text = text;
    sv.size = size;
    
    return sv;
}

String_View get_next_line(String_View* sv, size_t line_size) {
    
    if (line_size > sv->size) {
        line_size = sv->size;
    }

    String_View result = build_from_char(sv->text, line_size);

    sv->text  += line_size;
    sv->size -= line_size;

    return result;
}

String_View chop_by_delimiter(const char token, String_View* sv) {
  
  String_View result = { .text="", .size = 0};

  if (sv->size == 0) {
    return result;
  }


  for (int i = 0; i < sv->size; i++) {
    if (sv->text[i] == token) {
      
      result.text = sv->text;
      result.size = i;
      
      i++;
      sv->text += i;
      sv->size -= i;  
      return result;
    }
  }
  
  result.text = sv->text;
  result.size = sv->size;
  
  sv->size = 0;
    
  return result;
}

char* to_cstr(String_View sv) {
  size_t cstr_size = (sv.size + 1)*(sizeof(char*));  
  char* cstr = malloc(cstr_size);
  
  for (size_t i = 0; i < sv.size; i++)
    cstr[i] = sv.text[i];
  
  cstr[sv.size+1] = '\0';
  
  return cstr;
}



