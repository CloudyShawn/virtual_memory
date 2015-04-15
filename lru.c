

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// Keeps track of time frames
int counter;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	// Temporary variable for keeping track of oldest seen frame
	int time_dif = -1;
	int return_frame = -1;

	// Go through all frames to find oldest
	int i;
	for(i = 0; i < memsize; i++)
	{
		if(counter - coremap[i].stamp > time_dif)
		{
			time_dif = counter - coremap[i].stamp;
			return_frame = i;
		}
	}

	return return_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	//Stamp page with last used time frame
	coremap[p->frame >> PAGE_SHIFT].stamp = counter++;
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// Start time frame at 0
	counter = 0;
}

