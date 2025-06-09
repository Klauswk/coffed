#ifndef LIST_WINDOW_H_
#define LIST_WINDOW_H_

#include <time.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIST_SIZE 1000

typedef struct Node
{
  void *value;
  struct Node *next;
  struct Node *previous;
} Node;

typedef void (*free_node_callback)(Node *node);

typedef struct List
{
  int size;
  int max_size;
  Node *head;
  Node *tail;
  free_node_callback free_node;
} List;

#define FOR_EACH_IN_LIST(type, element, list, body)     \
  Node *n = list->head;                               \
  for (size_t index = 0; index < list->size; index++) \
{                                                   \
  type element = n->value;                        \
  body;                                           \
  n = n->next;                                    \
}

void init_list(List *list, int max_size, free_node_callback free_node);

void clear_list(List *list);

void add_to_list(List *list, void *value);

void remove_value_from_list(List *list, void *value);

Node *find_node_on_value(List *list, void *value);

Node *list_get_node_at(List *list, size_t position);

void *list_get_value_at(List *list, size_t position);

#endif
