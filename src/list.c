#include "list.h"

static void default_free_node(Node* node)
{
	free(node);
}

void init_list(List* list, int max_size, free_node_callback free_node) {
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	list->max_size = max_size;
	if(free_node)
		list->free_node = free_node;
	else
		list->free_node = default_free_node;
}

void clear_list(List* list)
{
	Node *value = list->head;
	Node *next = NULL;
	while (value != NULL)
	{
		next = value->next;
		list->free_node(value);
		value = next;
		list->size--;
	}

	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
}

void add_to_list(List* mainList, void *value)
{
	Node* newNode = malloc(sizeof(Node));
	newNode->value = value;
	newNode->next = NULL;
	newNode->previous = NULL;

	if (mainList->size >= MAX_LIST_SIZE)
	{
		Node* ptr = mainList->head->next;
		ptr->previous = NULL;
		mainList->free_node(mainList->head);
		mainList->head = ptr;
	} else {
		mainList->size++;
	}

	if (mainList->head == NULL)
	{
		mainList->head = newNode;
		mainList->tail = newNode;
		return;
	}

	Node* temp = mainList->head;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newNode;
	newNode->previous = temp;
	mainList->tail = newNode;
}

Node* find_node_on_value(List* list, void *value)
{
    Node* n = list->head;

    do {
		if(value == n->value)
		{
			return n;
		}
		n = n->next;
    } while(n != NULL);

	return NULL;
}

void remove_value_from_list(List* list, void *value)
{

	if(list->head == NULL)
	{
		return;
	}

    Node* current = list->head;

	if(current->value == value)
	{
		list->head = current->next;
		if(current->next)
		{
			current->next->previous = NULL;
		}
		list->free_node(current);
		list->size--;
		return;
	}
	
    do {
		if(current->value == value)
		{
			if(current->next != NULL)
			{
				current->next->previous = current->previous;
				current->previous->next = current->next; 
			}
			else
			{
				current->previous->next = NULL;
				list->tail = current->previous;
			}
			list->free_node(current);
			list->size--;
			return;
		}
        current = current->next;
    } while(current != NULL) ;
}

Node *list_get_node_at(List* list, size_t position) {
	Node* node = list->head;

	if(node == NULL) {
		return NULL;
	}

	for(size_t i = 0; i <= position; i++) {
		
		if(i == position) {
			return node;
		}
		
		if(node->next == NULL) {
			return NULL;
		}

		node = node->next;
	}

	return NULL;
}

void* list_get_value_at(List* list, size_t position) {
	Node* node = list->head;

	if(node == NULL) {
		return NULL;
	}

	for(size_t i = 0; i <= position; i++) {
		
		if(i == position) {
			return node->value;
		}
		
		if(node->next == NULL) {
			return NULL;
		}

		node = node->next;
	}

	return NULL;
}
