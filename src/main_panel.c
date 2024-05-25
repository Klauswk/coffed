#include "main_panel.h"

static void free_node(Node *node)
{
	free(node->value);
	free(node);
}

static bool add_file_to_list(Main_Panel *mp, char *file_location, char error_buffer[4096])
{
	FILE* fd = fopen(file_location, "r");

	if (fd == NULL)
	{
		printf("It was not possible to open the file %s: %s \n", (char *)file_location, strerror(errno));
		log_info("It was not possible to open the file %s : %s \n", (char *)file_location, strerror(errno));
		snprintf(error_buffer, 4096, "It was not possible to open the file %s : %s \n", (char *)file_location, strerror(errno));
		return false;
	}

    struct stat file_stat1;  
    int result = fstat (fileno(fd), &file_stat1);  

	if (result < 0)
	{
		printf("It was not possible to open the file %s: %s \n", (char *)file_location, strerror(errno));
		log_info("It was not possible to open the file %s : %s \n", (char *)file_location, strerror(errno));
		snprintf(error_buffer, 4096, "It was not possible to open the file %s : %s \n", (char *)file_location, strerror(errno));
		return false;
	}

	FOR_EACH_IN_LIST(FILE*, file, mp->list_file_descriptors, {

    	struct stat file_stat;  
    	int rStat = fstat (fileno(file), &file_stat);  

		if (rStat >= 0) {
			if (file_stat1.st_ino == file_stat.st_ino) {
				printf("The file %s is already open \n", file_location);
				log_info("The file %s is already open \n", file_location);
				snprintf(error_buffer, 4096, "The file %s is already open \n", file_location);
				return false;
			}
		}
	});


	fseek(fd, 0, SEEK_END);

	add_to_list(mp->list_file_descriptors, fd);

	return true;
}

static void put_message(Main_Panel* mp, const char* message, enum Message_Level level)
{
	show_message(&mp->mw, message, level);
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
			put_message(mp, "No file defined", ML_ERROR);
			return;
		}
		char file_path[BUFFER_SIZE] = "";

		memcpy(file_path, command + 9, BUFFER_SIZE - 9);
		
		char error_buffer[4096];

		if(!add_file_to_list(mp, file_path, error_buffer)) {
			put_message(mp, error_buffer, ML_ERROR);
		}
		return;
	} else if (command != NULL && strstr(command, ":remFile") != NULL) {
		if(strlen(command) < 10) {
			log_info("Got into no file defined\n");
			put_message(mp, "No file defined", ML_ERROR);
			return;
		}
		char file_number_s[BUFFER_SIZE] = "";

		memcpy(file_number_s, command + 9, BUFFER_SIZE - 9);

		long file_number = strtol(file_number_s, 0, 10);

		if (mp->list_file_descriptors->size == 1) {
			log_info("The program needs at least 1 file to follow\n");
			put_message(mp, "The program needs at least 1 file to follow", ML_ERROR);
			return;
		}

		if (file_number >= 0) {
			FILE* f = list_get_value_at(mp->list_file_descriptors, file_number); 
			if (f != NULL) 
				remove_value_from_list(mp->list_file_descriptors, f);
		}

		return;
	} else if (command != NULL && strstr(command, ":setM") != NULL) {
		if(strlen(command) < 7) {
			log_info("No message defined\n");
			put_message(mp, "No message defined", ML_ERROR);
			return;
		}
		char message[BUFFER_SIZE] = "";

		memcpy(message, command + 6, BUFFER_SIZE - 6);

		put_message(mp, message, ML_INFO);
		return;
	} else if( command != NULL && strstr(command, ":RESIZE_WINDOW\t")) {
		int row, col;

		getmaxyx(stdscr, row, col);
		resize_log_window(&mp->lw, row, col);
		resize_command_window(&mp->cw, row, col);
		return;
	} else if( command != NULL && strstr(command, ":SHOW_FILE_LIST\t")) {
		if (!mp->lfv.isShowing) {
			mp->lfv.isShowing = true;
			show_file_list(&mp->lfv, mp->list_file_descriptors);
			show_panel(mp->top);
		} else {
			mp->lfv.isShowing = false;
			hide_panel(mp->top);
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

	char error_buffer[4096];

	do
	{
		if(!add_file_to_list(&mp, n->value, error_buffer)) {
			return 1;
		}

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
		use_default_colors();
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);
	}

	int row, col;

	getmaxyx(stdscr, row, col);

	PANEL  *my_panels[2];

	mp.lw = create_log_window(row, col);
	mp.cw = create_command_window(row, col, BUFFER_SIZE, &mp, change_filter_status);
	mp.mw = create_message_window(row, col);
	mp.lfv = create_file_list_view(row, col);

	my_panels[0] = new_panel(mp.lw.window);
	my_panels[1] = new_panel(mp.lfv.border_win);

	hide_panel(my_panels[1]);

	set_panel_userptr(my_panels[0], my_panels[1]);
	set_panel_userptr(my_panels[1], my_panels[0]);
	
	mp.top = my_panels[1];

	Log log = {0};
	while (1)
	{
		update_panels();
		get_next_log(&log, ((FILE *)current_file->value));
		
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

		if(handle_input_command_window(&mp.cw) && mp.mw.is_showing) {
			clear_message(&mp.mw);
			wrefresh(mp.cw.window);
		}
		
		doupdate();
	}
	endwin();

	return 0;
}
