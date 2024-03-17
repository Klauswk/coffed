#include <stdio.h>
#include <stdarg.h>
#include "assert.h"
#include "command_history.h"

static void test_create()
{
    Command_History test_list = init_command_history(NULL);

    assert(test_list.capacity == 1000);
    assert(test_list.current_position == 0);
    assert(test_list.count == 0);

    free(test_list.list_of_commands);
}

static void test_add()
{
    Command_History test_list = init_command_history(NULL);

    add_to_command_history(&test_list, "Something");
    add_to_command_history(&test_list, "Something1");
    add_to_command_history(&test_list, "Something2");

    assert(test_list.capacity == 1000);
    assert(test_list.current_position == 3);
    assert(test_list.count == 3);

    for (int i = 0; i < test_list.count; i++)
    {
        free(test_list.list_of_commands[i]);
    }
    free(test_list.list_of_commands);
}

static void test_previous()
{
    Command_History test_list = init_command_history(NULL);

    add_to_command_history(&test_list, "Something");
    add_to_command_history(&test_list, "Something1");
    add_to_command_history(&test_list, "Something2");

    assert(test_list.capacity == 1000);
    assert(test_list.current_position == 3);
    assert(test_list.count == 3);

    {
        const char *previous = get_previous_command(&test_list);

        assert(strcmp("Something2", previous) == 0);
        assert(test_list.current_position == 2);
    }

    {
        const char *previous = get_previous_command(&test_list);

        assert(strcmp("Something1", previous) == 0);
        assert(test_list.current_position == 1);
    }

    {
        const char *previous = get_previous_command(&test_list);

        assert(strcmp("Something", previous) == 0);
        assert(test_list.current_position == 0);
    }

    {
        const char *previous = get_previous_command(&test_list);

        assert(strcmp("", previous) == 0);
        assert(test_list.current_position == 0);
    }
    
    {
        const char *previous = get_previous_command(&test_list);

        assert(strcmp("", previous) == 0);
        assert(test_list.current_position == 0);
    }

    for (int i = 0; i < test_list.count; i++)
    {
        free(test_list.list_of_commands[i]);
    }
    free(test_list.list_of_commands);
}

static void test_next()
{
    Command_History test_list = init_command_history(NULL);

    add_to_command_history(&test_list, "Something");
    add_to_command_history(&test_list, "Something1");
    add_to_command_history(&test_list, "Something2");

    assert(test_list.capacity == 1000);
    assert(test_list.current_position == 3);
    assert(test_list.count == 3);
    
    for(int i = 0; i < test_list.count + 1; i++) {
        get_previous_command(&test_list);
    }
    
    {
        const char *next = get_next_command(&test_list);

        assert(test_list.current_position == 1);
        assert(strcmp("Something1", next) == 0);
    }

    {
        const char *next = get_next_command(&test_list);

        assert(test_list.current_position == 2);
        assert(strcmp("Something2", next) == 0);
    }

    {
        const char *next = get_next_command(&test_list);

        assert(test_list.current_position == 2);
        assert(strcmp("", next) == 0);
    }

    {
        const char *next = get_next_command(&test_list);

        assert(test_list.current_position == 2);
        assert(strcmp("", next) == 0);
    }

    for (int i = 0; i < test_list.count; i++)
    {
        free(test_list.list_of_commands[i]);
    }
    free(test_list.list_of_commands);
}

int main(int argc, char **argv)
{
    RUN_FILE(argc, argv, {
        RUN_TEST(test_create);
        RUN_TEST(test_add);
        RUN_TEST(test_previous);
        RUN_TEST(test_next);
        // RUN_TEST(test_navigation);
    });
}