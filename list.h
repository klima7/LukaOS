#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>
#include <stdint.h>
#include "threads.h"

struct node_t
{
	THREAD *data;
	struct node_t* next;
	struct node_t* prev;
};

struct list_t
{
	struct node_t* head;
	struct node_t* tail;
    uint32_t size;
};

typedef struct list_t LIST;
typedef struct node_t NODE;

LIST *list_create(void);
void list_destroy(LIST *l);
void list_push_front(LIST *l, THREAD *data);
void list_push_back(LIST* l, THREAD *data);
THREAD *list_pop_front(LIST *l);
THREAD *list_pop_back(LIST *l);
THREAD *list_front(LIST *l);
THREAD *list_back(LIST *l);
THREAD *list_remove_node(LIST *l, NODE *node);

#endif