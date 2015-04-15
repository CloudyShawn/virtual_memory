

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	// Skip all frames that have been referenced
	while(coremap[clock_hand].pte->frame & PG_REF)
	{
		// Set referenced frame to not referenced
		coremap[clock_hand++].pte->frame &= ~PG_REF;

		// Reset clock hand when too high
		if(clock_hand == memsize)
		{
			clock_hand = 0;
		}
	}

	// Clock hand pointing at not referenced frame
	return clock_hand;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	// Find correct frame
	int pfn = p->frame >> PAGE_SHIFT;

	// Mark frame as referenced
	coremap[pfn].pte->frame |= PG_REF;
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	// Start clock hand at arbitrary point, might as well be 0
	clock_hand = 0;
}

