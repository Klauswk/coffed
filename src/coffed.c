#include <locale.h>
#include "main_panel.h"
#include "logger.h"

static void free_node(Node* node)
{
	free(node->value);
	free(node);
}

int main(int argc, char **argv)
{

	List* files = malloc(sizeof(List));
	init_list(files, argc, free_node);
	setlocale(LC_ALL, "");
	
	char c;

	#ifdef DEBUG
		FILE *log_file = fopen(LOG_PATH, LOG_TYPE);
		
		init_log(log_file);
		
		if (log_file == NULL)
		{
			printf("Something went wrong creating the log file\n");
			return 1;
		}
	#endif

	if(argc < 2)
	{
		printf("Is necessary to give at least one file to filter, use coffed file1 file2.... \n");
		log_info("Is necessary to give at least one file to filter, use coffed file1 file2.... \n");
		return 1;
	}

	for(int i = 1 ; i < argc; i++) 
	{
		size_t size = sizeof(char) * strlen(argv[i]);
        void* value = malloc(size);
        memcpy(value, argv[i], size);
		add_to_list(files, value);
	}

	return start_app(files);
}
