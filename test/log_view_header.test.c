#include <string.h>
#include "assert.h"
#include "log_view_header.h"

static void test_create()
{
    Log_View_Header* header = create_log_view_header(1,1, "Test");

    assert(header->current != NULL);
    assert(header->headers->size == 1);
    assert(header->current->is_main == 1);
    assert( strcmp(header->current->header, "Test") == 0);

    clear_list(header->headers);
    free(header->headers);
    free(header);
}

static void test_add_header_item()
{
    Log_View_Header* header = create_log_view_header(1,1, "Test");

    add_header_item(header, "Item 1");
    add_header_item(header, "Item 2");

    assert(header->current == header->headers->tail->value);

    Log_View_Header_Item* item1 = header->headers->head->next->value;
    assert(item1->is_main == 0);
    assert(strcmp(item1->header, "Item 1") == 0);
    Log_View_Header_Item* item2 = header->headers->head->next->next->value;
    assert(item2->is_main == 0);
    assert(strcmp(item2->header, "Item 2") == 0);
    
    clear_list(header->headers);
    free(header->headers);
    free(header);
}

static void test_remove_header_at_1_element()
{
    Log_View_Header* header = create_log_view_header(1,1, "Test");

    add_header_item(header, "Item 1");

    remove_header_at(header, header->headers->tail->value);

    assert(header->headers->head->next == NULL);
    
    clear_list(header->headers);
    free(header->headers);
    free(header);
}

static void test_remove_header_at_2_element()
{
    Log_View_Header* header = create_log_view_header(1,1, "Test");
    
    add_header_item(header, "Item 1");
    add_header_item(header, "Item 2");

    Log_View_Header_Item* item1 = header->headers->head->next->value;
    Log_View_Header_Item* item2 = header->headers->head->next->next->value;

    remove_header_at(header, item2);

    assert(header->headers->head->next->next == NULL);
    
    remove_header_at(header, item1);
    
    assert(header->headers->head->next == NULL);

    remove_header_at(header, header->headers->head->value);
    
    //This should be the main one, so it should be remove it.   
    assert(header->headers->head != NULL);

    clear_list(header->headers);
    free(header->headers);
    free(header);
}

int main(int argc, char** argv)
{
    
    RUN_FILE(argc, argv, {
        RUN_TEST(test_create);
        RUN_TEST(test_add_header_item);
        RUN_TEST(test_remove_header_at_1_element);
        RUN_TEST(test_remove_header_at_2_element);
    });
}