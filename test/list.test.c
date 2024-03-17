#include <stdio.h>
#include <stdarg.h>
#include "assert.h"
#include "list.h"

static void test_create()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    assert(test_list != NULL);
    assert(test_list->max_size == 10);
    assert(test_list->head == NULL);
    assert(test_list->size == 0);
    
    clear_list(test_list);
    free(test_list);
}

static void test_insert()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    char* anyValue = "This is a test";

    add_to_list(test_list, anyValue);

    assert(test_list->size == 1);
    assert(test_list->head->value == anyValue);
    assert(test_list->head->previous == NULL);
    assert(test_list->tail->value == anyValue);

    clear_list(test_list);
    free(test_list);
}

static void test_clear_list()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    const char *anyValue[4];

    anyValue[0] = "This is a test";
    anyValue[1] = "This is another value";
    anyValue[2] = "This is yet another value";
    anyValue[3] = "This is the value";

    add_to_list(test_list, &anyValue[0]);
    add_to_list(test_list, &anyValue[1]);
    add_to_list(test_list, &anyValue[2]);
    add_to_list(test_list, &anyValue[3]);

    clear_list(test_list);

    assert(test_list->head == NULL);
    assert(test_list->tail == NULL);
    assert(test_list->size == 0);

    free(test_list);
}

static void test_find()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    const char *anyValue[4];

    anyValue[0] = "This is a test";
    anyValue[1] = "This is another value";
    anyValue[2] = "This is yet another value";
    anyValue[3] = "This is the value";

    add_to_list(test_list, &anyValue[0]);
    add_to_list(test_list, &anyValue[1]);
    add_to_list(test_list, &anyValue[2]);
    add_to_list(test_list, &anyValue[3]);

    Node* node1 = find_node_on_value(test_list, &anyValue[0]);

    assert(node1->previous == NULL);
    assert(node1->next != NULL);
    assert(node1->value == &anyValue[0]);

    Node* node2 = find_node_on_value(test_list, &anyValue[2]);

    assert(node2->previous == node1->next);
    assert(node2->next != NULL);
    assert(node2->value == &anyValue[2]);

    Node* node3 = find_node_on_value(test_list, &anyValue[3]);

    assert(node3->previous == node2);
    assert(node3->next == NULL);
    assert(node3->value == &anyValue[3]);

    clear_list(test_list);
    free(test_list);

}

static void test_find_tail()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    const char *anyValue[4];

    anyValue[0] = "This is a test";
    anyValue[1] = "This is another value";
    anyValue[2] = "This is yet another value";
    anyValue[3] = "This is the value";

    add_to_list(test_list, &anyValue[0]);
    add_to_list(test_list, &anyValue[1]);
    add_to_list(test_list, &anyValue[2]);
    add_to_list(test_list, &anyValue[3]);

    Node* node2 = find_node_on_value(test_list, &anyValue[2]);

    Node* node4 = test_list->tail;

    assert(node4->previous == node2);
    assert(node4->next == NULL);
    assert(node4->value == &anyValue[3]);

    clear_list(test_list);
    free(test_list);

}

static void test_delete()
{
    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    const char *anyValue[4];

    anyValue[0] = "This is a test";
    anyValue[1] = "This is another value";
    anyValue[2] = "This is yet another value";
    anyValue[3] = "This is the value";

    add_to_list(test_list, &anyValue[0]);
    add_to_list(test_list, &anyValue[1]);
    add_to_list(test_list, &anyValue[2]);
    add_to_list(test_list, &anyValue[3]);

    // 0 <-> 1 <-> 2 <-> 3
    remove_value_from_list(test_list, &anyValue[0]);
    
    assert(test_list->head->previous == NULL);
    assert(test_list->head->next != NULL);
    assert(test_list->head->value == &anyValue[1]);

    // 1 <-> 2 <-> 3
    remove_value_from_list(test_list, &anyValue[2]);

    // 1 <-> 3
    assert(test_list->head->previous == NULL);
    assert(test_list->head->next != NULL);
    assert(test_list->head->value == &anyValue[1]);
    assert(test_list->head->next->value != NULL);
    assert(test_list->head->next->value == &anyValue[3]);
    
    Node* node3 = find_node_on_value(test_list, &anyValue[3]);
    
    assert(node3 != NULL);
    assert(node3->previous != NULL);
    assert(node3->previous->value == &anyValue[1]);

    clear_list(test_list);
    free(test_list);

}

static void test_list_for_each() {

    List* test_list = malloc(sizeof(List)); 
    init_list(test_list, 10, NULL);

    const char *anyValue[4];

    anyValue[0] = "This is a test";
    anyValue[1] = "This is another value";
    anyValue[2] = "This is yet another value";
    anyValue[3] = "This is the value";

    add_to_list(test_list, &anyValue[0]);
    add_to_list(test_list, &anyValue[1]);
    add_to_list(test_list, &anyValue[2]);
    add_to_list(test_list, &anyValue[3]);

    
    FOR_EACH_IN_LIST(const char**, value, test_list, {
        printf("Value : %s and anyValue[%ld]: %s\n", *value, index, anyValue[index]);
        assert(value == &anyValue[index]);
    });
}

int main(int argc, char** argv)
{
    RUN_FILE(argc, argv, {
        RUN_TEST(test_create);
        RUN_TEST(test_insert);
        RUN_TEST(test_clear_list);
        RUN_TEST(test_find);
        RUN_TEST(test_find_tail);
        RUN_TEST(test_delete);
        RUN_TEST(test_list_for_each);
    });
}