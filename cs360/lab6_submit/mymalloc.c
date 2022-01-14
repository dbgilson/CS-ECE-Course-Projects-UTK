//CS360 Lab 6: Jmalloc
//By: David Gilson
//The purpose of this code is to create a version of malloc.  This can be done by utilizing sbrk and a 
//struct that contains data about a free list that keeps track of where the free holes are in heap.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mymalloc.h"

//This is the free list struct that we put into free space
//to keep track of the free holes in heap.
typedef struct Flist {
	size_t size;
	struct Flist *next;
	struct Flist *prev;
} flist;

//One global pointer to the head of the free list is all we need.
flist *malloc_head = NULL;

//This is the malloc function that allocates memory using sbrk and the free list
//to track free spaces in the memory to fill.
void *my_malloc(size_t size) {
	
	// Align the size and the 8 bookkeeping bytes to a chunk
	size_t chunk = (size + 15) & -8;
	size_t min = 8192;
	size_t sbrk_size = min;
	
	//If size <= 0, just return NULL
	if (size <= 0){
		return NULL;
	}
	
	//Choose the greater between 8192 and the desired aligned chunk size to put
	//into sbrk
	if (chunk > min){
		sbrk_size = chunk; 
	}
	
	// If it is the first malloc, initialize free list
	if (malloc_head == NULL){
		malloc_head = (flist *) sbrk(sbrk_size);
		malloc_head->size = sbrk_size;
		malloc_head->next = NULL;
		malloc_head->prev = NULL;
	}


	flist *current_ptr = malloc_head;
	
	//Search free list for free space.  If not found, allocate new space
	//with sbrk.
	while (current_ptr->size < chunk){
		
		if (current_ptr->next != NULL){
			current_ptr = current_ptr->next;

		} 
		else{
			//We've reached end of free list, so allocate more
			current_ptr->next = (flist *) sbrk(sbrk_size);
			current_ptr->next->prev = current_ptr;
			current_ptr = current_ptr->next;
			current_ptr->next = NULL;
			current_ptr->size = sbrk_size;
		}
	}
	
	//If the size is big enough, parse the memory and return start of data point.
	//Otherwise, give it the entire node and remove node from free list.
	//Also make sure to return as void * ptr, not flist ptr or other.
	if (current_ptr->size >= chunk + 8){
		char *dstart = (char *) current_ptr;
	
		dstart += current_ptr->size;
		dstart -= chunk;
		*((int *) dstart) = chunk;
		dstart += 8;
		current_ptr->size -= chunk;

		return (void *) dstart;
	} 
	else{
		*((int *) current_ptr) = chunk;

		if (current_ptr == malloc_head){ 
			malloc_head = malloc_head->next;
		} 
		else{
			// Iterate through free list and disconnect full node.
			flist *temp = malloc_head;
			while (temp->next != current_ptr){
				temp = temp->next;
			}
			temp->next->prev = temp;
			temp->next = temp->next->next;
		}

		return ((void *) current_ptr) + 8;
	}
}

//This function adds the chunk at the given pointer to the free list.
//I decided to keep free relatively simple and handle most of the free list reordering and 
//relinking in coalesce_free_list().
void my_free(void *ptr) {
	if (ptr == NULL){ 
		return;
	}

	flist *temp = malloc_head;

	malloc_head = ptr - 8;
	malloc_head->next = temp;
	malloc_head->prev = NULL;
}

//All this function does is return head of the free list.
void *free_list_begin() {
	return malloc_head;
}

//This function returns the next node in the free list given a node pointer.
void *free_list_next(void *node) {
	if (node == NULL){
		return NULL;
	}

	return ((flist *) node)->next; 
}

//This function combines contiguous nodes in the free list.
void coalesce_free_list() {
	
	//Count nodes in free list.
	flist *current_node = free_list_begin();
	int ncount = 0;

	while (current_node != NULL) {
		ncount++;
		current_node = current_node->next;
	}

	//Store node addresses in array.  They need to
	//be unsigned int arrays so the ordering doesn't get 
	//messed up.
	unsigned int nodes[ncount], sorted[ncount];
	int i, j;
	
	current_node = free_list_begin();

	for (i = 0; i < ncount; i++) {
		nodes[i] = (unsigned int) current_node;
		current_node = current_node->next;
	}

	// Sort node list in ascending order. This will let us
	//easily reconnect them. I decided to just sort them this way
	//instead of using qsort() as this just seemed more straight forward
	//and let me relink the nodes.
	unsigned int min = nodes[0], min_index = 0;

	for (i = 0; i < ncount; i++) {
		for (j = 0; j < ncount; j++) {			
			if (nodes[j] <= min) {
				min = nodes[j];
				min_index = j;
			}
		}

		nodes[min_index] = -1;
		sorted[i] = min;
		min = -1;

		// Relink nodes to make coallescing easier.
		if (i != 0){
			((flist *) sorted[i])->prev = (flist*) sorted[i-1];
			((flist *) sorted[i-1])->next = (flist *) sorted[i];
		}
	}

	malloc_head = (flist *) sorted[0];
	((flist *) sorted[ncount - 1])->next = NULL;

	current_node = free_list_begin();

	flist *tnode;

	//Go through free list and merge when contiguous
	while (current_node != NULL) {

		//Check for NULL and if we were already at this node
		if (current_node->size == 0 || current_node->next == NULL) {
			current_node = current_node->next;
			continue;
		}

		tnode = current_node->next;

		// Merge if chunks are adjacent. Change size to 0 to keep from iterating through again.
		while ( ((void *) current_node) + current_node->size == tnode) {
			current_node->next = tnode->next;
			tnode->prev = current_node->prev;
			current_node->size += tnode->size;
			tnode->size = 0;
			tnode = tnode->next;
		}
		current_node = current_node->next;
	}
}

