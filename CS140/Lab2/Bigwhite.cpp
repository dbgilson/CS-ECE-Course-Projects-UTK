//CS140 Lab 2 Bigwhite || By: David Gilson
//The purpose of this code is to take user input of the number of rows and columns, and then make a pgm file of all white pixels of the size row*col.
//First include the libraries.
#include<iostream>
#include<vector>
#include<iomanip>
#include<cstdlib>
#include<cstdio>
#include<sstream>

using namespace std;

//Using command line arguments, it takes in the values given by the user, error checks them, and outputs a properly formatted pgm file from the given number
//of rows and columns.
int main(int argc, char *argv[]){
	
	istringstream value;
	int R = 0;
	int C = 0;
	
	//Here is some error checking code.  Code with fprintf is used for error checking to stderr.
	if(argc != 3){
		fprintf(stderr, "usage: bigwhite rows cols\n");
		return 0;
	}
	
	value.str(argv[1]);
	if(!(value >> R)){
		fprintf(stderr, "usage: bigwhite rows cols\n");
		return 0;
	}

	value.clear();

	value.str(argv[2]);
	if(!(value >> C)){
		fprintf(stderr, "usage: bigwhite rows cols\n");
		return 0;
	}
	
	if(C <= 0 || R <= 0){
		fprintf(stderr, "usage: bigwhite rows cols\n");
		return 0;
	}

	printf("P2\n");
	printf("%d %d\n", C, R);
	printf("255\n");

	for(int i = 0; i < R; i++){

		printf("255");

		for(int j = 0; j < C; j++){
			printf(" 255");
		}

		printf("\n");
	}

	return 0;
}

