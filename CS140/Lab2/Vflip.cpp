//CS140 Lab2 Vflip || By: David Gilson
//The purpose of this code is to read in a pgm file, flip its rows, and output the resulting pgm picture. (i.e. flipping the picture vertically)
//First is the libraries
#include<iostream>
#include<vector>
#include<iomanip>
#include<cstdlib>
#include<cstdio>

using namespace std;

//The code first reads in the header information of the pgm file (pgm, columns, rows, 255) and error checks.  It then puts the pixels in a 2D vector by rows and columns, and then
//"flips" the pixels from top to bottom in the outputting pgm by just reading out the vector from end to beginning.
int main(){
	vector<vector<int> > flipped;
	string P2;
	int R = 0;
	int C = 0;
	int num255 = 0;
	int row = 0;
	int col = 0;

	cin >> P2 >> C >> R >> num255;
	
	//Here is some of my error checking code.  Any code using fprintf is used for error checking and outputting to stderr.
	if(P2 != "P2"){
	  fprintf(stderr, "Bad PGM file -- Does not start with P2\n");
	  return 0;
	}
    	
    if(C <= 0 || R <= 0){
	  fprintf(stderr, "Bad PGM file -- Values less than or equal to 0\n");
	  return 0;
	}

	if(num255 != 255){
		fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
		return 0;
	}


	flipped.resize((R));

	for(row = 0; row < flipped.size(); row++){
		flipped[row].resize((C));
	}

	int pixelCount = 0;

	for(row = 0; row < flipped.size(); row++){
		for(col = 0; col < flipped[0].size(); col++){
			cin >> flipped[row][col];
			pixelCount++;
			if(flipped[row][col] < 0 || flipped[row][col] > 255){
				fprintf(stderr, "Bad PGM file -- pixel %d is not a number between 0 and 255\n", pixelCount - 1);
				return 0;
			}
		}
	}

	if((flipped.size() * flipped[0].size()) > (C*R)){
		fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
		return 0;
	}

	if((flipped.size() * flipped[0].size()) < (C*R)){
		fprintf(stderr, "Bad PGM file -- Not enough pixels\n");
		return 0;
	}

	printf("P2\n");
	printf("%d %d\n", C, R);
	printf("255\n");
	
	//This is the part that does the "flipping" by just outputting the vector in reverse order by rows, but not with columns.
	for(row = (R-1); row >= 0; row--){
		for(col = 0; col < flipped[0].size(); col++){
			if(col == C-1){
				printf("%d\n", flipped[row][col]);
			}
			else{
				printf("%d ", flipped[row][col]);
			}
		}
	}
	return 0;
}


