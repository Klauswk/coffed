#ifndef __COFFED_FORMATER__
#define __COFFED_FORMATER__

#include <stddef.h>
#include <stdio.h>

typedef char* (*Format_Callback)(const char* line,const size_t size);
typedef char* (*Format_Name_Version)();

typedef struct Formater_Plugin {
	Format_Callback callback;
	Format_Name_Version name_version_callback;
} Formater_Plugin;

typedef struct Log_File
{
	FILE* fd;	
	char* plugin_name;
} Log_File;


#endif
