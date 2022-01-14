//David Gilson :: Lab 4
//The purpose of this code is to create a new class called doublevector and implement functions for that class using pointers
//
//First we include our c++ libraries and using namespace
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <cstddef>
using namespace std;

//Now we make our class definition and subsequent private values and public functions to access/manipulate those values.
class doublevector{
	double *mValues;
	
	int mNumElements;

public:
	doublevector();

	~doublevector();

	void push_back(double x);

	void insert(double a, int b);

	void erase(int a);

	double *at(int x);

	const double *at(int x) const;

	int size() const;

};

//This is the constructer, setting mNumElements to 0 and mValues to NULL since it is a new value.
doublevector::doublevector(){
	mNumElements = 0;
	mValues = NULL;
}

//This is the decontructor, which clears the memory location of the values. 
doublevector::~doublevector(){
	delete &mNumElements;
	delete mValues;
}

//This function pushes a vector onto the bottom of the vector.  We have to make a new vector of +1 size, put the new value on the bottom, then copy the rest of//old vector onto the new one.  Then we delete the old vector and replace it with the new vector.
void doublevector::push_back(double x){
	 double *y = new double[mNumElements + 1];

	     y[0] = x;

		  for(int i = 1; i < mNumElements + 1; i++){
			  y[i] = mValues[i-1];
		  }

		 delete mValues;

		 mValues = y;
}

//This function inserts a value into a given index.  Similar to the pushback function, we make a new vector of +1 size, copy the old vector, insert the value
//at the index location, copy the rest of the old vector, delete the old vector, and replace the old vector location values with the new vector.
 void doublevector::insert(double a, int b){
		 double *y = new double[mNumElements + 1];

		 for(int i = 0; i < b; i++){
		 y[i] = mValues[i];
		 }

		 y[b] = a;

		 for(int j = b+1; j < mNumElements + 1; j++){
		 y[j]= mValues[j-1];
		 }

		 delete mValues;

		 mValues = y;
  }

//This function erases a value from the vector.  Similar to the pushback and insert function, we make a new vector, but this time it is -1 of the size of the
//old vector.  We copy the old vector, and when it gets to the index, it copies the next index value in its place, and continues to copy until the size is 
//filled.  We then delete the old vector and replace its location values with those of the new vector.
void doublevector::erase(int a){
	 double *y = new double[mNumElements - 1];
					       
	 for(int i = 0; i < a; i++){
	 y[i] = mValues[i];
	 }

	 for(int j = a; j < mNumElements; j++){
		 y[j] = mValues[j+1];
		 if((j+1) == mNumElements){
			 break;
		 }
	 }

	 delete mValues;

	 mValues = y;
}

//This function returns the pointer to a specific value of the vector.  It first checks to see if the value given is within the index, and if it is, it returns
//the pointer value.  If it does not lie within the bounds of the vector, the function returns NULL.
double *doublevector::at(int x){
	if((x >= 0) && (x <= (mNumElements - 1))){
	return &mValues[x];
	}
	
	else{
	return NULL;
	}
}

//Same as the function above, but it is for the constant classes.
const double *doublevector::at(int x) const{
     if((x >= 0) && (x <= (mNumElements - 1))){
	return &mValues[x];
	 }

	 else{
	 return NULL;
	 }
}

//This function simply returns the size of the vector using mNumElements
int doublevector::size() const{
	return mNumElements;
}

//This function prints out the values of the vector.  It uses the at function to know where to find the value.
void PrintVector(const doublevector *v){
	for(int i = 0; i < v->size(); i++){
		cout << *v->at(i) << " ";
	}
}

//This function copies the given doublevector into a new source vector of a different location.  It first checks to see if the destination vector exists, and
//if it does not, it creates a new doublevector.  It then copies the values of the old vector into the destination vector.
void CopyVector(doublevector **dst, const doublevector *src){
	int x = src->size();
    int y;

	    if(*dst == NULL){
		*dst = new doublevector;
		}

		else{
		y = (*dst)->size();
		}

	    for(int i = 0; i < x; i++) {
		if(i < y){
		 *dst[i] = src[i];
		}

		else if(i > y){
		  (*dst)->push_back(*src->at(i));
		 }
		
		 if(y > x){
		 (*dst)->erase(i+x);
		}
	}
}

//This function applies a function to the values of a given doublevector in order to change every value according to the function pointer given.
void Map(doublevector *v, double (*mapfunc)(double)){

	for(int i = 0; i < v->size(); i++){
		*v->at(i) = mapfunc(*v->at(i));
	}

}

//We finally call int main, but as we only want the functions, int main does nothing.
int main(){
}

