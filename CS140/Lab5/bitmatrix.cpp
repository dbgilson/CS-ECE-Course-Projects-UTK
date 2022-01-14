//CS140 Lab 5 Bit Matrices By: David Gilson
//The purpose of this code is to create and manipulate a "bit-matrix," which is basically a matrix of 0's and 1's.  Each element of the matrix is a char
//instead of an int.  Below are the class functions, the headers of which are found in "bitmatrix.h," which is included in the libraries of this file.
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "bitmatrix.h"

//This is the bitmatrix constructor.  It creates a bitmatrix of size rows*cols, and each element is initially made to '0'.
Bitmatrix::Bitmatrix(int rows, int cols){
	if(rows <= 0 || cols <= 0){
		fprintf(stderr, "Rows or cols is less than or equal to 0");
		M.resize(1);
		M[0].resize(1, '0');
	}
	else{
		M.resize(rows);
		for(int i = 0; i < M.size(); i++){
			M[i].resize(cols, '0');
		}
	}
}

//This function simply returns the number of rows in the pointed bitmatrix.
int Bitmatrix::Rows(){
	return M.size();
}

//This funciton simply returns the number of columns in the pointed bitmatrix.
int Bitmatrix::Cols(){
	return M[0].size();
}

//This function sets the value of the bitmatrix at M[row][col] to be the value (Either a '0' or '1').
void Bitmatrix::Set(int row, int col, char val){
	char acv;

	if(row < 0 || row >= M.size() || col < 0 || col >= M[0].size()){
        fprintf(stderr, "Bad inputs");
        exit(1);
    }
	
	acv = val;

	if(val == 0){
		acv = '0';
	}

	if(val == 1){
		acv = '1';
	}

	M.at(row).at(col) = acv;
}

//This function returns the value of the bitmatrix at index M[row][col].  It returns integers of 0 and 1, even though the type says it returns chars, 
//but the functions that reference this one accomodate this.
char Bitmatrix::Val(int row, int col){
	if(row >= M.size() || row < 0 || col >= M[0].size() || col < 0){
		return -1;
	}

	else if(M[row][col] == '0'){
		return 0;
	}

	else if(M[row][col] == '1'){
		return 1;
	}
}

//This function prints the bitmatrix on standard output.
void Bitmatrix::Print(int w){
  int i, j;

  for (i = 0; i < M.size(); i++) {
    if (w > 0 && i != 0 && i%w == 0) printf("\n");
    if (w <= 0) {
      cout << M[i] << endl;
    } else {
      for (j = 0; j < M[i].size(); j++) {
        if (j % w == 0 && j != 0) printf(" ");
			printf("%c", M[i][j]);
      }
      cout << endl;
    }
  }
}

//This function writes the bitmatrix to a file.
void Bitmatrix::Write(string fn){
	ofstream f;
	int i, j;

	f.open(fn.c_str());
	if(f.fail()){
		fprintf(stderr, "Failed to open file");
		exit(1);
	}

	for(i = 0; i < M.size(); i++){
		for(j = 0; j < M[i].size(); j++){
			f << M[i][j];
		}
		f << endl;
	}
	f.close();
}

//This function swaps the rows of the bitmatrix at rows r1 and r2.  This function uses the arithmetic function "swap" found in the arithmetic library.
void Bitmatrix::Swap_Rows(int r1, int r2){
	swap(M[r1], M[r2]);
}

//This function sets the row r1 of the bitmatrix to the sum of r1 and r2.  In this function, you have to convert the char values of the bitmatrix to ints,
//do the math, and then turn them back into chars.
void Bitmatrix::R1_Plus_Equals_R2(int r1, int r2){
	char temp;
	unsigned int t1, t2, tempi;

	if(r1 < 0 || r1 >= M.size() || r2 < 0 || r2 >= M.size()){
        fprintf(stderr, "Bad row inputs");
        exit(1);
    }
	
	else{
		for(int i = 0; i < M[0].size(); i++){
			t1 = M[r1][i] - '0';
			t2 = M[r2][i] - '0';
			tempi = (t1 + t2)%2;
			temp = tempi + '0';
			M[r1][i] = temp;
		}
	}
}

//This is an additional constructor that reads a bitmatrix from a file and sets the current bitmatrix to the one read.
Bitmatrix::Bitmatrix(string fn){
  ifstream f;
  int i, j;
  string s, row;

  f.open(fn.c_str());
  if (f.fail()) { perror(fn.c_str()); M.resize(1); M[0].resize(1), M[0][0] = '0'; return; }

  while (getline(f, s)) {
    row.clear();
    for (i = 0; i < s.size(); i++) {
      if (s[i] == '0' || s[i] == '1') {
        row.push_back(s[i]);
      } else if (s[i] != ' ') {
        fprintf(stderr, "Bad matrix file %s\n", fn.c_str());
        M.resize(1); M[0].resize(1), M[0][0] = '0';
        f.close();
        return;
      }
    }
    if (s.size() > 0 && M.size() > 0 && row.size() != M[0].size()) {
	  fprintf(stderr, "Bad matrix file %s\n", fn.c_str());
      M.resize(1); M[0].resize(1), M[0][0] = '0';
      f.close();
      return;
    }
    if (s.size() > 0) M.push_back(row);
  }
  f.close();
}

//This function writes a pgm file made from the current bitmatrix to a file.  Each "pixel" of the pgm is a pixel*pixel size, and black borders of size
//border can be added as well.
void Bitmatrix::PGM(string fn, int pixels, int border){
	ofstream f;
	int sRow = 0;
	int sCol = 0;
	int bcountr = 0;
	int bcountc = 0;

	vector<vector<int> > pgm;

	f.open(fn.c_str());

	if(f.fail()){
		fprintf(stderr, "Could not open file");
		exit(1);
	}
	
	//Resizing pgm to have all 0's at the right size.
	pgm.resize((M.size()*pixels)+(M.size()+1)*border);
	for(int i = 0; i < pgm.size(); i++){
		pgm[i].resize((M[0].size()*pixels)+(M[0].size()+1)*border, 0);
	}

	sRow = border;
	sCol = border;
	
	//Arithmetic process of putting pixels in correct places.
	for(int j = 0; j < M.size(); j++){
		for(int k = 0; k < M[j].size(); k++){
			if(M[j][k] == '0'){
				for(int l = 0; l < pixels; l++){
					for(int m = 0; m < pixels; m++){
						pgm[(border + l)+(bcountr*(pixels+border))][(border + m)+(bcountc*(pixels+border))] = 255;
					}
				}
			}

			if(M[j][k] == '1'){
				for(int n = 0; n < pixels; n++){
					for(int o = 0; o < pixels; o++){
						pgm[(border + n)+(bcountr*(pixels+border))][(border + o)+(bcountc*(pixels+border))] = 100;
					}
				}
			}
			bcountc++;
		}
		bcountr++;
		bcountc = 0;
	}

	f << "P2" << endl;
	f << pgm[0].size() << " " << pgm.size() << endl;
	f << "255" << endl;
	
	//Writing off the pgm file.
	for(int p = 0; p < pgm.size(); p++){
		for(int q = 0; q < pgm[0].size(); q++){
			if(q == pgm[0].size() - 1){
			    f << pgm[p][q] << endl;
			}
			else{
				f << pgm[p][q] << " ";
			}
		}
	}

	f.close();
}

//This function creates a new bitmatrix that is a copy of the current bitmatrix and returns the copy.
Bitmatrix *Bitmatrix::Copy(){

	Bitmatrix *N;
	N = new Bitmatrix(M.size(), M[0].size());

	for(int i = 0; i < M.size(); i++){
		for(int j = 0; j < M[i].size(); j++){
			N->M[i][j] = M[i][j];
		}
	}

	return N;
}

//These new functions are part of the BM_Hash class

//This function sets the hash table size to whatever size is given.
BM_Hash::BM_Hash(int size){
	table.resize(size);
}

//This function stores the bitmatrix and the given string.  The function first uses djb_hash to find the table index to use, then stores the key and bitmatrix in the table.
void BM_Hash::Store(string &key, Bitmatrix *bm){
	int i;
	unsigned int hash, index;
	int test = 0;
	int count = 0;

	hash = 5381;

	for(i = 0; i < key.size(); i++){
		hash = (hash << 5) + hash + key[i];
	}
	
	index = hash % table.size();
	
	if(table[index].size() == 0){

        HTE *temp;
        temp = new HTE;

        temp->key = key;
        temp->bm = bm;

        table[index].push_back(temp);
        return;
	}
	else{
	for(int j = 0; j < table[index].size(); j++){
		if(table[index][j]->key == key){
			table[index][j]->bm = bm;
			return;
		}
		if(count == table[index].size() - 1){
			HTE *temp;
			temp = new HTE;

			temp->key = key;
			temp->bm = bm;

			table[index].push_back(temp);
			return;
		}
		count++;
	}
	}

}

//This function returns the bitmatrix found in the hash table with the given key.  It uses the given key to find the table index using djb_hash, and if the key isn't found, it 
//returns NULL.
Bitmatrix *BM_Hash::Recall(string &key){
	int i;
    unsigned int hash, index;
    int test = 0;
    int count = 0;

    hash = 5381;

    for(i = 0; i < key.size(); i++){
        hash = (hash << 5) + hash + key[i];
    }

    index = hash % table.size();

	if(table[index].size() == 0){
		return NULL;
	}
	
	else{
    for(int j = 0; j < table[index].size(); j++){
        if(table[index][j]->key == key){
            return table[index][j]->bm;
        }
        if(count == table[index].size() - 1){
            return NULL;
        }
        count++;
    }
	}
}

//This function creates a HTVec of the hash table entries, which is then used to print out the table in another function.
HTVec BM_Hash::All(){
  HTVec rv;

  for(int i = 0; i < table.size(); i++){
	  for(int j = 0; j < table[i].size(); j++){
		  rv.push_back(table[i][j]);
	  }
  }

  return rv;
}

//This function adds the two bitmatrices together using matrix arithmetic (modding 2), and returns the summed bitmatrix.  This function converts the chars given by the bitmatrices
//into ints to do the calculations, and it then converts them back when putting the elements in the resulting bitmatrix.
Bitmatrix *Sum(Bitmatrix *m1, Bitmatrix *m2){
	char temp;
	unsigned int t1, t2, tempi;
	int sm1r, sm2r, sm1c, sm2c;

	sm1r = m1->Rows();
	sm1c = m1->Cols();
	sm2r = m2->Rows();
	sm2c = m2->Cols();

	if(sm1r != sm2r || sm1c != sm2c){
		return NULL;
	}

	Bitmatrix *bm;
	bm = new Bitmatrix(sm1r, sm1c);

	for(int j = 0; j < bm->Rows(); j++){
		for(int k = 0; k < bm->Cols(); k++){
			t1 = m1->Val(j, k) - '0';
			t2 = m2->Val(j, k) - '0';
			tempi = (t1+t2)%2;
			temp = tempi + '0';
			bm->Set(j, k, temp);
		}
	}
	return bm;
}

//This function multiplies the tow bitmatrices together using matrix arithmetix (modding 2 when all values are added), and returns the product matrix.  Similar to sum, this function 
//converts the chars of the bitmatrices into ints to calculate and back into chars when setting the product bitmatrix's elements.
Bitmatrix *Product(Bitmatrix *m1, Bitmatrix *m2){
	char temp;
	unsigned int t1, t2, tempi;

	if(m1->Cols() != m2->Rows()){
		return NULL;
	}

	Bitmatrix *bm;
	bm = new Bitmatrix(m1->Rows(), m2->Cols());
	
	for(int i = 0; i < m1->Rows(); i++){
		for(int j = 0; j < m2->Cols(); j++){
			tempi = 0;
			for(int k = 0; k < m2->Rows(); k++){
				t1 = m1->Val(i, k) - '0';
				t2 = m2->Val(k, j) - '0';
				tempi += t1*t2;
			}
			tempi = tempi%2;
			temp = tempi + '0';
			bm->Set(i, j, temp);
		}
	}
	return bm;
}

//This function creates a bitmatrix that is made from the rows of the given matrix specified by the vector of row indexs.  Rows can be repeated.  The row size of the returning bitmatrix
//is the size of the vector of row indexes.
Bitmatrix *Sub_Matrix(Bitmatrix *m, vector <int> &rows){
	int max = m->Rows();
	int min = 0;

	for(int i = 0; i < rows.size(); i++){
		if(rows.size() == 0 || rows[i] > (max - 1) || rows[i] < 0){
			return NULL;
		}
	}

	Bitmatrix *bm;
	bm = new Bitmatrix(rows.size(), m->Cols());

	for(int j = 0; j < rows.size(); j++){
		for(int k = 0; k < m->Cols(); k++){
			bm->Set(j, k, m->Val(rows[j], k));
		}
	}

	return bm;
}

//This function creates the matrix-inverse of the bitmatrix given.  It first makes a copy of the bitmatrix and an identity bitmatrix, and uses the Swap_rows function and
//R1_Plus_Equals_R2 function in a specific manner on both of the created bitmatrices.  By the end of the function, the identity bitmatrix will be the inverse of the given bitmatrix, 
//and the function returns this inverse bitmatrix.
Bitmatrix *Inverse(Bitmatrix *m){
  int i, j;
	if(m->Rows() != m->Cols()){
		return NULL;
	}

	Bitmatrix *bm, *Inv;
	Inv = new Bitmatrix(m->Rows(), m->Cols());

	bm = m->Copy();

	for(i = 0; i < m->Rows(); i++){
		Inv->Set(i, i, '1');
	}

	int test = 0;
	int count = 1;

	for(i = 0; i < bm->Rows(); i++){
		if(bm->Val(i, i) != 1){
			test = 0;
			count = 1;
			while(test == 0){	
				if(count == (bm->Rows() - i)){
					return NULL;
				}
				if(bm->Val(i+count, i) == 1){
					bm->Swap_Rows(i, i+count);
					Inv->Swap_Rows(i, i+count);
					test = 1;
				}
				count++;
			}
		}
	
		for(j = i+1; j < bm->Rows(); j++){
			if(bm->Val(j, i) == 1){
				bm->R1_Plus_Equals_R2(j, i);
				Inv->R1_Plus_Equals_R2(j, i);
			}
		}
	}

	for(i = (bm->Rows()-1); i >= 0; i--){
		for(j = i+1; j < bm->Rows(); j++){
			if(bm->Val(i, j) == 1){
				bm->R1_Plus_Equals_R2(i, j);
				Inv->R1_Plus_Equals_R2(i, j);
			}
		}
	}

	return Inv;
}

