#ifndef DSET_H
#define DSET_H

#include <stdlib.h>

typedef struct NODE {
	int rank;
	int parent;
} node;

typedef struct DSET {
	int Nsets;
	node *S;
} dset;

//Node helper function
void node_setup(node *);

//Dset Helper Function
void dset_setup(dset *, int);
void dset_free(dset *);
int dset_size(dset *);
int dset_merge(dset *, int, int);
int dset_find(dset *, int);

#endif
