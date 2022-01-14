//CS140 Lab2 Neg || By: David Gilson
//The purpose of this code is to take a pgm file, invert the pixel values, and output the inverted pgm picture.
//First include the libraries.
#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<cstdio>

using namespace std;

//First reading in the P2 header, columns, rows, and 255, the code then just subtracts the pixel value from 255 to get the inverted output.  It does not
//store all of the pixel values, so it reads in the pixel value into 1 variable, inverts it, and then prints it out, saving memory.
int main(){
	string P2;
    int R = 0;
    int C = 0;
    int Num255 = 0;
    int pixel = 0;
	int i = 1;

    cin >> P2 >> C >> R >> Num255;
	
	//Here is some error checking codes for the input.  Any code using fprintf is meant to check for errors and print them out to stderr.
	if(P2 != "P2"){
		fprintf(stderr, "Bad PGM file -- Does not start with P2\n");
		return 0;
	}

	if(C <= 0 || R <= 0){
		fprintf(stderr, "Bad PGM file -- Values less than 0\n");
		return 0;
	}

	if(Num255 != 255){
		fprintf(stderr, "Bad PGM file -- No 255 following the rows and columns\n");
		return 0;
	}


    printf("P2\n");
	printf("%d %d\n", C, R);
	printf("255\n");

	int count = 0;
	while(cin >> pixel){
		if(pixel < 0){
			fprintf(stderr, "Bad PGM file -- Pixel value is less than 0\n");
			return 0;
		}

		pixel = 255 - pixel;

		if((i%C) == 0){
			printf("%d\n", pixel);
			i = 0;
		}

		if(i > 0){
			printf("%d ", pixel);
		}

		i++;
		count++;
	}

	if(count > (R*C)){
		fprintf(stderr, "Bad PGM file -- Extra stuff after the pixels\n");
		return 0;
	}

	if(count < (R*C)){
		fprintf(stderr, "Bad PGM file -- Not enough pixels\n");
		return 0;
	}


	return 0;
}

