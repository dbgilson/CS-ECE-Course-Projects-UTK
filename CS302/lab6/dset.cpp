#include "dset.h"
#include <stdlib.h>
#include <stdio.h>

void node_setup(node *n){
	n->rank = 0;
	n->parent = -1;
}

void dset_setup(dset *ds, int x) {
	ds->Nsets = x;
	ds->S = new node[x];
}

void dset_free(dset *ds){
	delete ds->S;	
}

int dset_size(dset *ds){
	return ds->Nsets;
}

int dset_merge(dset *ds, int i, int j) {
	i = dset_find(ds, i);
	j = dset_find(ds, j);
	if (i != j) {
		node *Si = &(ds->S[i]);
		node *Sj = &(ds->S[j]);
		
		// merge (union) by rank
		if (Si->rank > Sj->rank)      Sj->parent = i;
		else if (Si->rank < Sj->rank) Si->parent = j;
		else { Sj->parent = i; Si->rank += 1; }
		
		ds->Nsets--;
	}  return dset_find(ds, i);
}

int dset_find(dset *ds, int i) {
	//printf("got into dset_find\n");

	if (ds->S[i].parent == -1)
		return i;
	//printf("got past first if statement\n");
	// recursive path compression
	ds->S[i].parent = dset_find(ds, ds->S[i].parent);

	//printf("got to return statement\n");
	return ds->S[i].parent;
}
