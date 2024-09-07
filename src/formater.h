#ifndef __COFFED_FORMATER__
#define __COFFED_FORMATER__

#include <stddef.h>

typedef char* (*Format_Callback)(const char* line,const size_t size);

typedef struct Log_File
{
	FILE* fd;	
	Format_Callback callback;
} Log_File;


#endif
