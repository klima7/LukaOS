#include <stddef.h>
#include <stdint.h>
#include "list.h"
#include "heap.h"
#include "threads.h"

// Tworzy liste
LIST *list_create(void)
{
    LIST *list = (LIST*)kmalloc(sizeof(LIST));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// Zwalnia liste
void list_destroy(LIST *l)
{
    NODE *current = l->head;
    while(current!=NULL)
    {
        NODE *delete = current;
        current = current->next;
        kfree(delete);
    }

    kfree(l);
}

// Dodaje element na koniec listy
void list_push_back(LIST* l, THREAD *data)
{
	NODE* node = (NODE*)kmalloc(sizeof(NODE));

	node->data = data;
	node->next = NULL;
	node->prev = l->tail;

	if (l->tail == NULL)
	{
		l->head = node;
		l->tail = node;
	}
	else
	{
		l->tail->next = node;
		l->tail = node;
	}

    l->size++;
}

// Dodaje element na początek listy
void list_push_front(LIST *l, THREAD *data)
{
	NODE *node = (NODE*)kmalloc(sizeof(NODE));

	node->data = data;
	node->next = l->head;
	node->prev = NULL;

	if (l->head == NULL)
	{
		l->head = node;
		l->tail = node;
	}
	else
	{
		l->head->prev = node;
		l->head = node;
	}

    l->size++;
}

// Usówa i zwraca pierwszy element listy
THREAD *list_pop_front(LIST *l)
{
    if(l->size == 0) return 0;

	NODE* poped = l->head;
	THREAD *data = poped->data;

	if (l->head->next != NULL)
	{
		l->head = l->head->next;
		l->head->prev = NULL;
	}
	else
	{
		l->head = NULL;
		l->tail = NULL;
	}

	kfree(poped);
	return data;
}

// Usówa i zwraca ostatni element listy
THREAD *list_pop_back(LIST *l)
{
    if(l->size == 0) return 0;

	NODE* poped = l->tail;
	THREAD *data = poped->data;

	if (l->tail->prev != NULL)
	{
		l->tail = l->tail->prev;
		l->tail->next = NULL;
	}
	else
	{
		l->head = NULL;
		l->tail = NULL;
	}

	kfree(poped);
	return data;
}

// Zwraca pierwszy element listy
THREAD *list_front(LIST *l)
{
    if(l->size == 0) return 0;

	THREAD *data = l->head->data;
	return data;
}

// Zwraca ostatni element
THREAD *list_back(LIST *l)
{
    if(l->size == 0) return 0;

	THREAD *data = l->tail->data;
	return data;
}

// Usuwa i zwraca dany węzeł
THREAD *list_remove_node(LIST *l, NODE *node)
{
	if(l->head == node)
		l->head = node->next;
	else
		node->prev->next = node->next;

	if(l->tail == node)
		l->tail = node->prev;
	else 
		node->next->prev = node->prev;

	l->size--;

	THREAD *thread = node->data;
	kfree(node);
	
	return thread;
}