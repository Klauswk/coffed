#ifndef FORMATTER_PLUGIN_HASH_MAP
#define FORMATTER_PLUGIN_HASH_MAP

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "formater.h"
#define MAX_SIZE 5 

typedef struct {
	char * key;
	Formater_Plugin plugin; 
} Hash_Pair;

typedef struct {
	Hash_Pair values[MAX_SIZE];	
} Hash_Map;

int hash(const char* key);

int put_value(Hash_Map* hash_map, const char* key, Formater_Plugin plugin);

Format_Callback get_value(Hash_Map* hash_map, const char* key);

int delete_value(Hash_Map* hash_map, const char * key);

#endif
