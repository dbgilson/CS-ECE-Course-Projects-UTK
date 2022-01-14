//C140 Lab2 By: David Gilson
//The purpose of this code is to read a pgm file and print the number of rows, columns, pixels, and the average pixel value.

//First is the libraries
#include<iostream>
#include<vector>
#include<iomanip>
#include<cstdlib>
#include<cstdio>

using namespace std;

//I declare all of the variables, read in the P2 value, rows, columns, and 255 after that.  I then error check, read in the pixels, and print out the number
//of rows, columns, pixels, and the calculated average of the pixels.
int main(){
	string P2;
	string check;
	double averageP = 0;
	int pixelCount = 0;
	int R = 0;
	int C = 0;
	int Num255 = 0;
	int pixel = 0;

	cin >> P2 >> C >> R >> Num255;
	
	//This is error checking code.  Everything with fprintf is error checking.
	if(P2 != "P2"){
		fprintf(stderr, "Bad PGM file -- first word is not P2\n");
		return 0;
	}

	if(C <= 0 || R <= 0){
		fprintf(stderr, "Bad PGM file -- Values less than or equal to 0\n");
		return 0;
	}

	if(Num255 != 255){
		fprintf(stderr, "Bad PGM file -- 255 is not after the Row/Column line\n");
		return 0;
	}

	while(cin >> pixel){
		pixelCount++;

		averageP += pixel;

		if(pixelCount > (C*R)){
			fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
			return 0;
		}

		if(pixel < 0 || pixel > 255){
			fprintf(stderr, "Bad PGM file -- pixel %d is not a number between 0 and 255\n", pixelCount - 1);
			return 0;

		}
	}

		if(pixelCount < (C * R)){
			fprintf(stderr, "Bad PGM file -- Not enough pixels\n");
			return 0;
		}


	averageP = (averageP / pixelCount);

	printf("# Rows:    %8d\n", R);
	printf("# Columns: %8d\n", C);
	printf("# Pixels:  %8d\n", pixelCount);
	printf("Avg Pixel: %8.3f\n", averageP);
	
	return 0;
}
