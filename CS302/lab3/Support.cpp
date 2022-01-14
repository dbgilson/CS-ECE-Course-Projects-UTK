//CS 302, Lab 3
//By: David Gilson
//This is the Support.cpp file that contains the function definitions found in the Support.h header file made in this lab.

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <numeric>
#include <functional>
using namespace std;

#include "Support.h"

const char ETX = 0x3;

//Here is the random number generating functions for version 2 of the lab.
rnumgen::rnumgen(int seedvalue, vector<int> &v) {  
	srand(seedvalue);  
	F.resize(v.size());  
	partial_sum(v.begin(), v.end(), F.begin());  
	transform(F.begin(), F.end(), F.begin(),  bind2nd(divides<float>(), F.back()));
}
int rnumgen::rand() {  
	const double randmax = RAND_MAX+1.0;  
	const double p = (double)std::rand()/randmax;  
	return upper_bound(F.begin(), F.end(), p) - F.begin();
}

//Pixel constructor
pixel::pixel(int _x, int _y){
	x = _x;
	y = _y;
}

//RGB constructor
RGB::RGB(uchar _r, uchar _g, uchar _b){
	r = _r;
	g = _g;
	b = _b;
}

//Functions to get rows/columns of the image.
int ppm::get_Nrows() const{
	return pixels->get_Nrows();
}

int ppm::get_Ncols() const{
	return pixels->get_Ncols();
}

//Here is the read function for the ppm class.  It reads in the P6 ppm header information and the byte information to store them as pixels.
void ppm::read(const char* fname) { 
  ifstream fp;
  string p6;
  int depth;
  int w, h, x, y;

  fp.open(fname);

  if(!(fp.is_open())){
	  cerr << "Error: File could not be open to be read\n";
	  return;
  }

  //read in header stuff
  fp >> p6 >> w >> h >> depth;

  fp.get();

  new_image(w, h);

  for(y = 0; y < h; y++){
	  for(x = 0; x < w; x++){
		  RGB &ref = (*pixels)[x][y];
		  ref.r = fp.get();
		  ref.g = fp.get();
		  ref.b = fp.get();
	  }
  }
  fp.close();
}

//Here is the write function for the ppm class.  It creates a ppm file from another ppm file, but it outputs it is "filename_wmsg.ppm" as to
//indicate that it has the encoded message in it.
void ppm::write(const char* fname) { 
  int rows = get_Nrows();
  int cols = get_Ncols();

  string fileout = fname;
  fileout.erase(fileout.find('.'));
  fileout.append("_wmsg.ppm");
  ofstream fout(fileout.c_str());

  if(!(fout.is_open())){
	  cerr << "Error: File could not be open to write\n";
	  return;
  }

  fout << "P6\n" << cols << " " << rows << endl << "255\n";

  for(int i = 0; i < rows; i++){
	  for(int j = 0; j < cols; j++){
		  fout << (*pixels)[j][i].r << (*pixels)[j][i].g << (*pixels)[j][i].b;
	  }
  }

  fout.close();
}

//Here is a helper function that creates a new pixel RGB matrix whenver it is called.
void ppm::new_image(int w, int h){
	if(pixels != NULL){
		delete pixels;
	}
	pixels = new matrix<RGB>(w, h);
}
