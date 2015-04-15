

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

typedef struct node{
	int pfn;
	struct node *next;
	struct node *prev;
} node;

typedef struct stack {
	node *head;
	node *tail;
} stack;

stack *lru_list;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	node *temp = lru_list->head;
	int return_pfn = temp->pfn;

	lru_list->head = lru_list->head->next;

	if(lru_list->head == NULL)
	{
		lru_list->tail = NULL;
	}
	else
	{
		lru_list->head->prev = NULL;
		if(lru_list->head == lru_list->tail)
		{
			lru_list->tail->prev = NULL;
		}
	}

	free(temp);
	
	return return_pfn;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int pfn = p->frame >> PAGE_SHIFT;

	if(lru_list->head == NULL)
	{
		node *new_node = malloc(sizeof(node));
		new_node->pfn = pfn;
		new_node->prev = NULL;
		new_node->next = NULL;

		lru_list->head = new_node;
		lru_list->tail = new_node;
	}
	else
	{
		node *temp = lru_list->head;
		for(temp = lru_list->head; temp != NULL; temp = temp->next)
		{
			if(temp->pfn == pfn)
			{
				break;
			}
		}

		if(temp == NULL)
		{
			node *new_node = malloc(sizeof(node));
			new_node->pfn = pfn;
			new_node->prev = lru_list->tail;
			new_node->next = NULL;

			lru_list->tail = new_node;
		}
		else
		{
			lru_list->tail = temp;
			temp->prev = temp->next;
		}
	}

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	lru_list = malloc(sizeof(stack));
	lru_list->head = NULL;
	lru_list->tail = NULL;
}

