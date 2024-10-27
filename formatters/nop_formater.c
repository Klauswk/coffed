#include <stdlib.h>
#include <string.h>
#include "../src/formater.h"

char* nop_plugin_format(const char* line, const size_t size) {
	char* new_line = malloc(sizeof(char*) * size);
	memcpy(new_line, line, size);
	return new_line;
}

char* nop_plugin_name() {
	return "Default 0.1";
} 


