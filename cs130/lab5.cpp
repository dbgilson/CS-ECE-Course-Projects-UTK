//Lab 5
//First we access libraries, and we call using namespace std
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
using namespace std;

//These are the C++ versions of the functions
//There is bubblesort, which taking char size arrays and ordering them least to greatest in the array by value
//Selectionsort takes short size arrays and orders them least to greatest by value
//Insertionsort takes int size arrays and orders them least to greatest by value
//GreatestSum takes the largest value in the array, multiplies it by 2, sums all of the values, and returns the value as a long
void c_BubbleSort(char *array, int size);

void c_SelectionSort(short *array, int size);

void c_InsertionSort(int *array, int size);

long c_GreatestSum(int *array, int size);

//This code is necessary to link the assembly versions of the code
extern "C" { //Need extern "C" so the linker can see this from assembly w/o name mangling.
	void BubbleSort(char *array, int size);
	void SelectionSort(short *array, int size);
	void InsertionSort(int *array, int size);
	long GreatestSum(int *array, int size);
}

int main(){
	//Here I made the test code. I just commented it out so that it will show how I tested the functions.  I tested them first with the c++ functions (Which worked), and then I 
	//tested my assembly functions (with varying results)
/*
	char array[4] = {'b','a','d','c'};
	 
	  for(int i = 0; i < 4; i++){
	      cout << array[i] << "  ";
	      }
	 
	      BubbleSort(array, 4);
	
	      for(int j = 0; j < 4; j++){
	          cout << array[j] << "  ";
	          }
		  cout << endl;

	          short array1[4] = {4, 5, 3, 7};
	 
	          for(int i = 0; i < 4; i++){
	              cout << array1[i] << "  ";
	              }
	 
	             SelectionSort(array1, 4);
	 
	              for(int j = 0; j < 4; j++){
	                  cout << array1[j] << "  ";
	                  }
	
	                  int array2[4] = {3, 2, 5, 1};
	 
					  cout << endl;

	                  for(int i = 0; i < 4; i++){
	                      cout << array2[i] << "  ";
	                      }
	 
	                      InsertionSort(array2, 4);
	 
	                      for(int j = 0; j < 4; j++){
	                          cout << array2[j] << "  ";
	
						  }
			cout << endl;
	int array3[4] = {3, 2, 5, 1};

	for(int i = 0; i < 4; i++){
		    cout << array3[i] << "  ";
	}

	cout << GreatestSum(array3, 4) << endl;
*/
}

//Here is the bubblesort.  It takes an array and switches an index's value if the one to the right of it (above it) is greater.
void c_BubbleSort(char *array, int size){
	int temp;
		    
	for(int i = 0; i < size; i++){
		for(int j = 0; j < (size-i-1); j++){
		temp = array[j];
		array[j] = array[j+1];
		array[j+1] = temp;
		}
	}
}

//SelectionSort goes through the entire array, comparing an index values.  If i = 0, that means it is comparing all the array values to array[0], and if a value is smaller, it switches
//them.
void c_SelectionSort(short *array, int size){
	for (int i = 0; i < size; ++i){
		for (int j = i+1; j < size; ++j){
			if (array[i] > array[j]){
			array[i] = array[i]+array[j];
			array[j] = array[i]-array[j];
			array[i] = array[i]-array[j];
			}	
		}
	}	
}

//Insertionsort uses a kind of "sublist" that it compares the values of the array to.  If the value is smaller, it adds it to the front of the index.  If the value is larger, it adds
//it to the back of the index.
void c_InsertionSort(int *array, int size){
	   int j, temp;
	      
		for (int i = 0; i < size; i++){
		j = i;
		while (j > 0 && array[j] < array[j-1]){
		temp = array[j];
		array[j] = array[j-1];
		array[j-1] = temp;
		j--;
	  }
	}
}

//GreatestSum just adds all the values in an array, except the largest value of the array is doubled.  To get the largest value, it is easy to call a sorting function and just take
//the last index and double it.  Returns the sum as a long.
long c_GreatestSum(int *array, int size){
	    long sum = 0;
		c_InsertionSort(array, size);
			    
		for(int i = 0; i < (size - 1); i++){
		sum += array[i];
		}
		sum += (array[size - 1] * 2);
		return sum;
}

