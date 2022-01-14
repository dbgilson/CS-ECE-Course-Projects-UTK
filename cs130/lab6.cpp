//Lab 6 
//Purpose: Make a structure with functions in both cpp and assembly
//
//First we include the libraries and using namespace std (I just paste the libraries from previous iterations so that I dont have to write them all out)
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

//Now we use extern C to connect our assembly functions to this file.
extern "C" {
	void asm_Construct(void *mymap_pointer);
	void asm_Set(void *mymap_pointer, char key, int value);
	int  asm_Get(const void *mymap_pointer, char key);
	void asm_Erase(void *mymap_pointer, char key);
	void asm_Print(const void *mymap_pointer);
}

//Here are the function prototypes for the cpp versions of the functions.  Function definitions will be after int main
void c_Construct(struct mymap *mymap_pointer);
void c_Set(struct mymap *mymap_pointer, char key, int value);
int  c_Get(const struct mymap *mymap_pointer, char key);
void c_Erase(struct mymap *mymap_pointer, char key);
void c_Print(const struct mymap *mymap_pointer);

//Here we make the struct KeyValue, which is just made up of a char and int value.
struct KeyValue{
	char key;
	int value;
};

//Here we make the struct mymap, which has a KeyValue struct array.  The struct function use the assembly functions.
struct mymap {
	KeyValue mKeyValues[20];

	mymap () { asm_Construct(this); }
	void set(char key, int value) { asm_Set(this, key, value); }
	int get(char key) const { return asm_Get(this, key); }
	void erase(char key) { asm_Erase(this, key); }
	void print() const { asm_Print(this); }
};

//Here we call int main.  I am leaving it blank as the code i tested it with is very messy, so I just decided to leave it out to have a cleaner look.
int main(){
	    cout << "hello";
}

//Here we have the definition for the construct function, where it just makes the keyvalues of each mymap index the key '*' and value -1.
void c_Construct(struct mymap *p){
	for(int i = 0; i < 20; i++){
		(*p).mKeyValues[i].key = '*';
		(*p).mKeyValues[i].value = -1;
	}
}

//Here we have the definition for the Set function, where it will find the mymap index that has the key of the key given and puts the value given in the value at that index.  If it 
//doesn't find the index with the key given, it will put that value in one index with the key '*'.  Otherwise it does nothing.
void c_Set(struct mymap *p, char key, int value){
	    int placeset = -1;

		for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
				(*p).mKeyValues[i].value = value;
				return;
			}
			if((*p).mKeyValues[i].key == '*'){
				placeset = i;
			}
		}
	(*p).mKeyValues[placeset].key = key;
	(*p).mKeyValues[placeset].value = value;
}

//Here we have the definition for the Get function, where it finds the mymap index with the key given and returns that index's KeyValue value.  If it does not find it, it returns
//-1
int  c_Get(const struct mymap *p, char key){
	    for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
				return((*p).mKeyValues[i].value);
			}	
		}
	return(-1);
}

//Here we have the definition for the Erase function, where it finds the mymap index with the key given and replaces the KeyValue key with '*', but it does not change the KeyValue 
//value.
void c_Erase(struct mymap *p, char key){
	    for(int i = 0; i < 20; i++){
			if((*p).mKeyValues[i].key == key){
			(*p).mKeyValues[i].key = '*';
		}
	}
}

//Here we have the definition for the Print function, where it prints the entire mymap structure with "Key at index i = Value at index i".
void c_Print(const struct mymap *p){
	for(int i = 0; i < 20; i++){
		printf("%c = %d", (*p).mKeyValues[i].key, (*p).mKeyValues[i].value);
	}
}
