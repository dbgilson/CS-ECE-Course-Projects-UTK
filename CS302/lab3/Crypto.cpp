//CS 302, Lab 3
//By: David Gilson
//This is the main file for the lab.  It contains the encoding, decoding, pixel indexing code, and int main.  The purpose of this code is to take a ppm file of
//P6 format and create another ppm image that contains a hidden message.  This hidden message is made by taking in the message from cin values and changing the 
//bit values of the image to essentially "hide" the message in image.  The actual image will not look that different, but we can decode the newly created image
//bit values to "read" the hidden message.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

#include "Support.h"

//Here is the encoding function for the lab.  I takes the ppm image and the pixel_list to know what pixel index values to encode.  It reads the message in one
//char at a time through cin.get() and then encodes all the bits except the MSB into the LSB of the pixel values determined by the pixel_list.
void encode(ppm &image, const vector<pixel> &pix) {
  char c;
  int x, y, i_col, i_pix, i_bit, insert;

  i_col = 0;
  i_pix = 0;
  insert = 1;

  while (insert == 1){

	  if (!cin.get(c)){
		//if at end, insert a ETX character
		  c = ETX;
		  insert = 0;
	  }
	
	  for(i_bit = 0; i_bit < 7; i_bit++){

		  x = pix[i_pix].x;
		  y = pix[i_pix].y;

		  //Get right pixel and color channel
		  RGB &ref = image[x][y];

		  switch (i_col){
			  case 0:
				  //red pixel
				  ref.r &= 0xFE;
				  ref.r |= ((c >> i_bit) & 0x1);
				  break;
			  case 1:
				  //green pixel
				  ref.g &= 0xFE;
				  ref.g |= ((c >> i_bit) & 0x1);
				  break;
			  case 2:
				  //blue pixel
				  ref.b &= 0xFE;
				  ref.b |= ((c >> i_bit) & 0x1);
				  break;
		  }

		i_col++;
		if(i_col > 2){
			i_col = 0;
		}
		i_pix++;
	  }
	}
}

//This is the decoding function for the lab.  It takes in the ppm image that has the message, as well as the pixel_list to know which indexes to look at.  It goes through the
//image at each pixel index, taking the LSB of a pixel color to create 7 bits of an 8-bit char.  After the char is created, it is written out via cout.  Once it finds the
//ETX character, it stops decoding.
void decode(ppm &image, const vector<pixel> &pix) {
	char c;
	int x, y, i_col, i_pix, i_bit;

	i_col = 0;
	i_pix = 0;

	while(true){
		c = 0x0;
		
		//Go through the LSBs of the given RGB colors to make the char value.
		for(i_bit = 0; i_bit < 7; i_bit++){

			x = pix[i_pix].x;
			y = pix[i_pix].y;

			RGB &ref = image[x][y];

			switch (i_col){
				case 0:
					//red pixel
					c |= (ref.r & 0x1) << i_bit;
					break;
				case 1:
					//green pixel
					c |= (ref.g & 0x1) << i_bit;
					break;
				case 2:
					//blue pixel
					c |= (ref.b & 0x1) << i_bit;
					break;
			}

			i_col++;
			if(i_col > 2){
				i_col = 0;
			}
			i_pix++;
		}
		
		//If we found the ETX character, stop decoding.
		if(c == ETX){
			return;
		}
		cout << c;
	}
}

//Here is the pixel list code for version 1 of the lab, where it just wanted us to encode every even pixel value.
/*
void set_pixel_list(ppm &image, vector<pixel> &pix){
	int x, y, w, h;
	w = image.get_Ncols();
	h = image.get_Nrows();

	for(y = 0; y < h; y = y + 2){
		for(x = 0; x < w; x = x + 2){
			pix.push_back(pixel(x, y));
		}
	}
}
*/

//Here is the pixel list code for version 2 of the lab. It first pushes back all the pixel index values of the image, and then it uses a histogram, along with random
//number generation, to randomly decide what pixel index values to use.
void set_pixel_list(ppm &image, vector<pixel> &pix){
	int x, y, w, h, r30;
	w = image.get_Ncols();
	h = image.get_Nrows();
	vector<int>histogram;
	int colour;

	for(y = 0; y < h; y++){
		for(x = 0; x < w; x++){
			pix.push_back(pixel(x, y));
		}
	}

	histogram.resize(1 << 15, 0);

	for(y = 0; y < h; y++){
		for(x = 0; x < w; x++){
			RGB &ref = image[x][y];

			colour = (((ref.r >> 3) << 10) | ((ref.g >> 3) << 5) | (ref.b >> 3));

			histogram[colour]++;
		}
	}

	rnumgen rng(0, histogram);

	for(int i = (int)pix.size() - 1; i > 0; --i){
		r30 = (rng.rand() << 15) | rng.rand();

		swap(pix[i], pix[r30 % (i +1)]);
	}
}

//Here is int main.  In here, we check the command line code to check if it is properly made.  Then it takes in the image, creates a pixel index of the image, and then encodes
//or decodes this image based on what was given in the command line.  If encode is chosen, it writes the encoded ppm to a new ppm file.  If decode is chosen, it decodes the
//message and displays in via cout.
int main(int argc, char *argv[]) {
  if(argc != 3){
		cerr << "usage: ./crypto1 -encode|decode image.ppm" << endl;
		return 0;
	}
	if(strcmp(argv[1], "-encode") && strcmp(argv[1], "-decode")){
		cerr << "usage: ./cyrpto1 -encode|decode image.ppm" << endl;
		return 0;
	}
  ppm img;
  vector<pixel> prox_pix;

  string fname = argv[2];

  img.read(fname.c_str());
  set_pixel_list(img, prox_pix);

  //See if encoding or decoding
  if(!strcmp(argv[1], "-encode")){
	encode(img, prox_pix);
	img.write(fname.c_str());
  }
  else{
	decode(img, prox_pix);
  }
  return 0;
}
