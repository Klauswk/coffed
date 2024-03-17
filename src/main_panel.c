#include "main_panel.h"

static void free_node(Node *node)
{
	free(node->value);
	free(node);
}

static bool add_file_to_list(Main_Panel *mp, char *file_location)
{
	FILE* fd = fopen(file_location, "r");

	if (fd == NULL)
	{
		printf("It was not possible to open the file %s \n", (char *)file_location);
		log_info("It was not possible to open the file %s \n", (char *)file_location);
		return false;
	}

	fseek(fd, 0, SEEK_END);

	add_to_list(mp->list_file_descriptors, fd);

	return true;
}

static void put_message(Main_Panel* mp, const char* message)
{
	show_message(&mp->mw, message);
	getyx(mp->cw.window, mp->cw.cursor_y, mp->cw.cursor_x);
	wmove(mp->cw.window, mp->cw.cursor_y, mp->cw.cursor_x);
}

static void change_filter_status(void *main_panel, char *command)
{
	Main_Panel *mp = (Main_Panel *)main_panel;

	if (command != NULL && strstr(command, ":addFile") != NULL)
	{
		if(strlen(command) < 10) {
			log_info("Got into no file defined\n");
			put_message(mp, "No file defined");
			return;
		}
		char file_path[BUFFER_SIZE] = "";

		memcpy(file_path, command + 9, BUFFER_SIZE - 9);

		if(!add_file_to_list(mp, file_path)) {
			put_message(mp, "It was not able to open the file");
		}
		return;
	}

	set_filter_log_window(&mp->lw, command);
}

int start_app(List *files)
{

	Main_Panel mp = {0};
	mp.list_file_descriptors = malloc(sizeof(List));

	init_list(mp.list_file_descriptors, files->size, free_node);

	if (files == NULL || files->size == 0)
	{
		printf("Is necessary to give at least one file to filter, use coffed file1 file2.... \n");
		log_info("Is necessary to give at least one file to filter, use coffed file1 file2.... \n");
		return 1;
	}

	Node *n = files->head;

	do
	{
		add_file_to_list(&mp, n->value);

		n = n->next;
	} while (n != NULL);

	n = mp.list_file_descriptors->head;

	Node *current_file = mp.list_file_descriptors->head;

	// NCURSES LIB START
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	if (has_colors())
	{
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);
		init_pair(3, COLOR_WHITE, COLOR_RED);
	}

	int row, col;

	getmaxyx(stdscr, row, col);

	mp.lw = create_log_window(row, col);
	mp.cw = create_command_window(row, col, BUFFER_SIZE, &mp, change_filter_status);
	mp.mw = create_message_window(row, col);

	Log log = {0};

	while (1)
	{
		get_next_log(&log, ((FILE *)current_file->value));
		// fprintf(log_file,"read %d bytes from file.\n", nbytes);
		if (log.count > 0)
		{
			getyx(mp.cw.window, mp.cw.cursor_y, mp.cw.cursor_x);
			process_log_window(&mp.lw, log.line, log.count);
			wmove(mp.cw.window, mp.cw.cursor_y, mp.cw.cursor_x);
			
			log.count = 0;
		}
		else
		{
			if (current_file->next == NULL)
			{
				current_file = mp.list_file_descriptors->head;
			}
			else
			{
				current_file = current_file->next;
			}
		}

		if(handle_input_command_window(&mp.cw)) {
			clear_message(&mp.mw);
			wrefresh(mp.cw.window);
		}
	}
	endwin();

	return 0;
}
