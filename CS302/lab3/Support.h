//CS 302, Lab 3
//By: David Gilson
//This is the support header file for the crypto.cpp and support.cpp files.  It contains the class/structs for the lab without defining them, which is 
//Support.cpp's job.
#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>

typedef unsigned char uchar;
extern const char ETX;

//Here is the random number generation class for version 2 of the lab.
class rnumgen {  
	public:    
		rnumgen(int seedvalue, std::vector<int> &v);    
		int rand();  
	private:    
		std::vector<float> F;
};

//Here is the Matrix class.  Following the lab session, I switched the Rows and Column instances to the pixel class follow a (width, height) structure.  Basically,
//it was to make it easier to understand.
template <typename T>
class matrix { 
	public:
		//Swap Rows/Cols from handout
		matrix(int n_Ncols, int n_Nrows){
			Ncols = n_Ncols;
			Nrows = n_Nrows;

			buf = new T *[Ncols];
			for (int i = 0; i < Ncols; i++){
				buf[i] = new T[Nrows];
			}
		}
		
		~matrix(){
			for(int i = 0; i < Ncols; i++){
				delete [] buf[i];
			}
			delete [] buf;
		}

		int get_Nrows() const{
			return Nrows;
		}

		int get_Ncols() const{
			return Ncols;
		}

		T * operator[] (int i) {
			return buf[i];
		}

	private:
		int Nrows, Ncols;
		T **buf;
};

struct pixel {
	pixel(int = 0, int = 0);
	int x, y;
};

struct RGB {
	RGB(uchar = 0, uchar = 0, uchar = 0);
	uchar r, g, b;
};

//Here is the ppm class.  The function are defined in Support.cpp, but here they are listed.  The only functions completed here are the ppm constructor and
//the overidden [] operator.
class ppm {
	public:
		ppm(){
			pixels = NULL;
		}

		void read(const char*);
		void write(const char*);

		int get_Nrows() const;
		int get_Ncols() const;

		RGB * operator[](int j){
			return (*pixels)[j];
		}

	private:
		matrix<RGB> *pixels;

		void new_image(int, int);
};

#endif
