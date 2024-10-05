#include "hash_map.h"

int hash(const char* key) {
	long hash = 0;
	int key_size = strlen(key);

	for (size_t size = 0; size < key_size; size++) {
		hash = hash + key[size];
	}
	
	return (hash * 31) % MAX_SIZE;
}

int put_value(Hash_Map* hash_map, const char* key, Formater_Plugin* plugin) {
	int hash_key = hash(key);

	if (hash_map->values[hash_key].key == NULL) {
		char* pair_key = malloc(sizeof(char*) * strlen(key) + 1);
		memcpy(pair_key, key, strlen(key) + 1); 
		hash_map->values[hash_key].key = pair_key;
		hash_map->values[hash_key].plugin = plugin;
		return 0;
	}
	
	if (strcmp(hash_map->values[hash_key].key, key) == 0) {
		hash_map->values[hash_key].plugin = plugin;
		return 0;
	}
	
	for (size_t i = 0; i < MAX_SIZE; i++) {
		if (hash_map->values[i].key == NULL) {
			char* pair_key = malloc(sizeof(char*) * strlen(key));
			memcpy(pair_key, key, strlen(key) + 1); 
			hash_map->values[i].key = pair_key;
			hash_map->values[i].plugin = plugin;
			return 0;
		} else if (strcmp(hash_map->values[i].key, key) == 0) {
			hash_map->values[hash_key].plugin = plugin;
			return 0;
		} 

	}
	fprintf(stderr, "No space in the hashmap\n");
	return 1;
}

Format_Callback get_value(Hash_Map* hash_map, const char* key) {
	int hash_key = hash(key);

	if (hash_map->values[hash_key].key == NULL) {
		return NULL;
	}
	
	if (strcmp(hash_map->values[hash_key].key, key) == 0) {
		return hash_map->values[hash_key].plugin->callback;
	} else {
		for (size_t i = 0; i < MAX_SIZE; i++) {
			if (hash_map->values[i].key != NULL && strcmp(hash_map->values[i].key, key) == 0) {
				return hash_map->values[i].plugin->callback;
			}
		}
	}
	return NULL;
}

int delete_value(Hash_Map* hash_map, const char * key) {
	int hash_key = hash(key);

	if (hash_map->values[hash_key].key != NULL && strcmp(hash_map->values[hash_key].key, key) == 0) {
		free(hash_map->values[hash_key].key);
		free(hash_map->values[hash_key].plugin);
		hash_map->values[hash_key].key = NULL;
		hash_map->values[hash_key].plugin = NULL;
		return 0;
	}

	for (int i = 0; i < MAX_SIZE; i++) {

		if (hash_map->values[i].key != NULL && strcmp(hash_map->values[i].key, key) == 0) {
			free(hash_map->values[i].key);
			free(hash_map->values[i].plugin);
			hash_map->values[i].key = NULL;
			hash_map->values[i].plugin = NULL;
			return 0;
		}
	}
	
	return 1; 
}


