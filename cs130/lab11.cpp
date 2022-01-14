//Lab 11  By David Gilson
//The purpose of this code is to take the binary code from a .wav audio file, scale the volume from the command line arguments, and write the data back into another file.

//First we include the necessary libraries and include using namespace std.
#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <cstdlib>

using namespace std;

//This is the header struct to use for taking in the header data.
struct Header{
	char id[4];
	int size;
};

//This is the format struct to use for taking in the format data.
struct format{
	short wFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	short wBitsPerSample;
};

//Now we start int main, and include the command line arguments.
int main(int argc, char *argv[]){
	//Create the file pointer for use in opening the file given 
	FILE *fin;

	//Check if there are enough arguments given, if not, end the program
	if(argc < 4){
		printf("Usage: %s <file.bmp>\n", argv[0]);
		return -1;
	}

	//Open the wav file (1st argument)
	fin = fopen(argv[1], "rb");

	//If there is an issue opening the file, end the program
	if (fin == nullptr){
		perror(argv[1]);
		return -1;
	}

//Create a header struct for the main header of the file
Header main;

//Read in the bytes of the main header, and if the header id isn't "RIFF", end the program.
fread(&main, 1, sizeof(8), fin);
if(strncmp(main.id, "RIFF", 4)){}
	else{
		printf("There was an error with the Main Header ID");
		return -1;
	}

//Make a char array of 4 bytes to read in the WAVE heading, then use fread to read it in.
char wave[4];

fread(&wave[4], 1, sizeof(4), fin);

//Here are the variables we will use later, including an array of headers, a format struct, a few ints for checking and manipulating the array of headers, and a data array for the 
//data values.
struct Header h[10];
format y;
int check;
int i = 0;
short *data = new short[h[i].size];
Header hdata;

//I make a while loop to read in and check each heading, and to deal with the following bytes accordingly.
while(true){
check = fread(&h[i], 1, sizeof(8), fin);
if(check == 0){
	break;
	}

//If the header reads "fmt", put the next 16 bytes into a format struct. If the format tag doesn't read 0x0001, then end the program since the file format is wrong.
if(strncmp(h[i].id, "fmt ", 4)){
	fread(&y, 1, sizeof(16), fin);
	
	if(y.wFormatTag != 0x0001){
	printf("The file is in the wrong format");
	return -1;
	}
}

//If the header reads "data", read the bytes into the data array of shorts since the left/right scales are shorts.
else if(strncmp(h[i].id, "data", 4)){
	fread(data, sizeof(short), h[i].size, fin);
	hdata = h[i];
}

//If the header reads anything else, it just takes the size of that chunk and skips it to the next header.
else{
	fseek(fin, h[i].size, SEEK_CUR);
}
}

//Close the file, as there is no longer a need to keep it open.
fclose(fin);

//This converts the argument values given into ints in order to scale the data values with.
int left = atoi(argv[3]);
int right = atoi(argv[4]);

//Scaling the data. If the data index is even or 0, then scale it with the left scale.  If the index is odd, then scale it with the right scale.
for(int j = 0; j < hdata.size; j++){
	//if left side
	if(((j+2)%2) == 0){
		data[i] = data[i]*left;
	}

	if(((j+2)%2) == 1){
		data[i] = data[i]*right;
	}
}

//Create the file pointer in order to write the data back out onto the file given.
FILE *fout;

//Open the receiving file, and if there is an error opening the file, end the program.
fout = fopen(argv[2], "wb");

    if (fout == nullptr){
		perror(argv[1]);
		return -1;
	}

//Write all of the headers, format, and data onto the fout file.
fwrite(&main, 1, sizeof(8), fout);
fwrite(&wave, 1, sizeof(4), fout);
fwrite(&y, 1, sizeof(16), fout);
fwrite(&data, 1, sizeof(hdata.size), fout);

//Make sure to delete the dynamic memory made earlier.
delete data;

//Close the fout file.
fclose(fout);

//End the program
return 0;
}
