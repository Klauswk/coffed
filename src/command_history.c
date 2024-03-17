#include "command_history.h"

static bool increase_history_capacity(Command_History* ch, size_t amount)
{
    if(ch->count >= ch->capacity)
    {
        size_t new_size = ch->capacity + amount;
        ch->capacity = new_size; 
        ch->list_of_commands = realloc(ch->list_of_commands, sizeof(char*) * new_size);

        if(ch->list_of_commands == NULL)
        {
            return false;
        }
    }

    return true;
}

Command_History init_command_history(const char* command_file) {

    Command_History ch = {0};

    if(command_file != NULL) {
        FILE* command_history_file = NULL;
        const char *homedir = getenv("HOME");
        char* path_to_file = NULL;
        if(homedir == NULL) {
            command_history_file = fopen(command_file, "a+");
            path_to_file = malloc(strlen(command_file) + 1);
            strcpy(path_to_file, command_file);
        } else {
            size_t command_size = strlen(command_file) + 1;
            size_t home_path_size = strlen(homedir) + 1; // null terminator and forward slash

            path_to_file = malloc(command_size + home_path_size);

            strcpy(path_to_file, homedir);
            strcat(path_to_file, "/");
            strcat(path_to_file, command_file);

            command_history_file = fopen(path_to_file, "a+");
        }
        
        if(command_history_file==NULL)
        {
            log_info("Couldnt open the file %s for the history: %s\n", path_to_file, strerror(errno));
        } else {
            char * line = NULL;
            size_t len = 255;
            ssize_t read;
            
            while ((read = getline(&line, &len, command_history_file)) != -1) {
                //Avoid reading the \n
                line[read - 1] = 0;
                add_to_command_history(&ch, line);
            }

            fclose(command_history_file);
            
            command_history_file = fopen(path_to_file, "a");

            free(path_to_file);

            if(command_history_file==NULL)
            {
                log_info("Couldnt open the file %s for the history: %s\n", command_file, strerror(errno));
            } else { 
                ch.file_location = command_history_file;
            }
        }
    }

    if(!increase_history_capacity(&ch, 1000))
        exit(1);

    return ch;
}

void add_to_command_history(Command_History* command_history, const char* command) {

    if(command_history->count >= command_history->capacity) {
        if(!increase_history_capacity(command_history, 1000))
            exit(1);
    }

    if(command_history->file_location != NULL) {
        log_info("Writing history to file %s", command_history->file_location);
        fprintf(command_history->file_location, "%s\n" ,command);
        fflush(command_history->file_location);
    }

    size_t command_size = strlen(command) + 1;
    char* history_command = malloc(sizeof(char*) * command_size);
    
    memcpy(history_command, command, command_size);

    command_history->current_position = command_history->count + 1;
    command_history->list_of_commands[command_history->count++] = history_command;

}

const char* get_previous_command(Command_History* command_history) {
    if(command_history->current_position - 1 < 0) {
        return "";
    }
    return command_history->list_of_commands[--command_history->current_position];
}

const char* get_next_command(Command_History* command_history) {
    if((command_history->current_position + 1) >= command_history->count) {
        return ""; 
    } 
    return command_history->list_of_commands[++command_history->current_position];
}