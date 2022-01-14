//CS140 Lab2 Hflip || By: David Gilson
//The purpose of this code is to read in a pgm picture, flip the pixel values in each row, and outputting the flipped pgm. (i.e. flipping the picture horizontally)
//First include the libraries.
#include<iostream>
#include<vector>
#include<iomanip>
#include<cstdlib>
#include<cstdio>

using namespace std;

//The code reads in the header info (P2, columns, rows, and 255), error checks, then reads in a row into a 1D vector.  After calculating the reverse of the 
//vector, it then prints it out to the output, then the code reads the next row into the vector, replacing the previous values.  It does this until no 
//rows remain.
int main(){
	vector <int> flipped;
	string P2;
	int C = 0;
	int R = 0;
	int Num255 = 0;
	int pixel = 0;
	int i = 0;
	int j = 0;
	int tempVal = 0;
	

	cin >> P2 >> C >> R >> Num255;
	
	//Here is some of my error checking code.  All the code using fprintf is error checking code that outputs the error to stderr.
	if(P2 != "P2"){
		fprintf(stderr, "Bad PGM file -- first word is not P2\n");
		return 0;
	}

	if(C <= 0 || R <= 0){
		fprintf(stderr, "Bad PGM file --- Values are less than or equal to 0.\n");
		return 0;
	}

	if(Num255 != 255){
		fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
		return 0;
	}

	flipped.resize(C*R);

	int rowCount = 1;

	printf("P2\n");
	printf("%d %d\n", C, R);
	printf("255\n");
	
	int pixelCount = 0;

	while(true){
	
		for(int k = 0; k < C; k++){
			cin >> pixel;
			pixelCount++;
			if(pixel < 0 || pixel > 255){
				fprintf(stderr, "Bad PGM file -- pixel %d is not a number between 0 and 255\n", pixelCount - 1);
				return 0;
			}
			flipped.at(k) = pixel;
		}

		//This particular portion is what flips the vector values
		for(int l = 0; l < (C/2); l++){
			tempVal = flipped.at(l);
			flipped.at(l) = flipped.at((C-1) - l);
			flipped.at((C-1) - l) = tempVal;
		}

		for(int m = 0; m < C; m++){
			if(m == (C-1)){
				printf("%d\n", flipped.at(m));
			}
			else{
				printf("%d ", flipped.at(m));
			}
		}
		if(rowCount == R){
			break;
		}

		rowCount++;
	}

	return 0;
}


