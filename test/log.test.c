#include <stdio.h>
#include "assert.h"
#include "log.h"

void test_read_empty_file()
{
    Log log = {0};

    FILE *fp = fopen("empty_file.txt","r");

    if(fp == NULL) {
        assert(0,"Couldnt open file");
    }

    if(!get_next_log(&log, fp)) {
        assert(0,"Failed to read content from file");
    }

    assert(log.count == 0);
}

void test_read_multiple_lines()
{
    Log log = {0};

    FILE *fp = fopen("example_test.txt","r");

    if(fp == NULL) {
        assert(0,"Couldnt open file");
    }

    if(!get_next_log(&log, fp)) {
        assert(0,"Failed to read content from file");
    }

    assert(log.count == 15);
    
    log.count = 0;

    if(!get_next_log(&log, fp)) {
        assert(0,"Failed to read content from file");
    }

    assert(log.count == 21);
    
    log.count = 0;
    
    if(!get_next_log(&log, fp)) {
        assert(0,"Failed to read content from file");
    }

    assert(log.count == 446);
    
    log.count = 0;
    
    free(log.line);
}

int main(int argc, char** argv)
{
    RUN_FILE(argc, argv, {
        RUN_TEST(test_read_multiple_lines);
        RUN_TEST(test_read_empty_file);
    });
}