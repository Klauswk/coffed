#include "log.h"

static bool increase_log_capacity(Log* log, size_t amount)
{
    if(log->count >= log->capacity)
    {
        size_t new_size = log->capacity + amount;
        log->capacity = new_size; 
        log->line = realloc(log->line, sizeof(char*) * new_size);

        if(log->line == NULL)
        {
            return false;
        }
    }

    return true;
}

bool get_next_log(Log* log, FILE* file)
{
    if (file == NULL) {
        return false;
    }
	size_t nbytes = 0;
	char buf[BUFFER_SIZE] = {0};
    size_t current_pos = ftell(file);

    while(1)
    {
        nbytes = fread(buf, 1, BUFFER_SIZE, file);
        
        //printf("Bytes read: %zu\n", nbytes);
        //printf("Buffer: %.*s\n",nbytes, buf);


        if (nbytes == -1 || ferror(file))
        {
            log_info("An error occour while reading the file\n");

            return false;
        }

        if(nbytes == 0) {
            if(log->count != 0) {
                log->line[log->count++] = '\0';
            }
            return true;
        }

        if(nbytes > 0)
        {
            if(nbytes + log->count >= log->capacity)
            {
                if(!increase_log_capacity(log, nbytes)) {
                    exit(1);
                }
            }

            for(int i = 0; i < nbytes; i++) {
                if (buf[i] == '\n') {
                    log->line[log->count++] = '\0';
                    fseek( file, current_pos + log->count, SEEK_SET );
                    return true;
                } else {
                    log->line[log->count++] = buf[i];
                }
            }
        }
    }

    return true;
}