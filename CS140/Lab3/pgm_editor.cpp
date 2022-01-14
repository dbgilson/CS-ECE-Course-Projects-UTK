#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
using namespace std;

typedef vector <int> IVec;

// Write your code here:
// CS140 Lab 3 || By: David Gilson
// The purpose of this lab was to define and prototype various functions that would manipulate a given 2D pgm vector, therefore manipulating the picture.

//This function actually writes out the pgm vector that it is given to standard output.
void pgm_write(vector <IVec> &p){
	int rows = 0;
	int cols = 0;
	int count = 0;

	rows = p.size();
	cols = p[0].size();

	printf("P2\n");
	printf("%d %d\n", cols, rows);
	printf("255\n");

	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			if(count == 20){
				printf("\n");
				count = 0;
			}

			printf("%4d", p[i][j]);
			count++;
		}
	}
	printf("\n");
}

//This function creates a 2D pgm vector based off of the given row, column, and pixel value.
vector<IVec> pgm_create(int r, int c, int pv){
	vector<IVec> p;

	p.resize(r);
	for(int i = 0; i < r; i++){
		p[i].resize(c);
	}
	
	for(int j = 0; j < r; j++){
		for(int k = 0; k < c; k++){
			p[j][k] = pv;
		}
	}

	return p;
}

//This function rotates the pgm picture clockwise by switching row and column values in a specific manner.  The number of rows become the number of columns
//and vice versa, and then the first column of the original pgm becomes the first row of the rotated pgm, but in inverse order.
void pgm_cw(vector <IVec> &p){
	int rSize;
	int cSize;

	rSize = p.size();
	cSize = p[0].size();

	vector<vector<int> > cw;

	cw.resize(cSize);

	for(int i = 0; i < cw.size(); i++){
		cw[i].resize(rSize);
	}
	
	int count = 0;
	for(int j = 0; j < cw.size(); j++){
		for(int k = cw[0].size() - 1; k >= 0; k--){
			cw[j][count] = p[k][j];
			count++;
			if(count == cw[0].size()){
				count = 0;
			}
		}
	}

	p.resize(cSize);

	for(int l = 0; l < p.size(); l++){
		p[l].resize(rSize);
	}

	for(int m = 0; m < p.size(); m++){
		for(int n = 0; n < p[0].size(); n++){
			p[m][n] = cw[m][n];
		}
	}

}

//This function rotates the pgm counter clockwise by switching the rows and columns in a specific manner.  The number of rows become the number of columns
//and vice versa, and the last column of the original pgm becomes the first row of the rotated pgm.
void pgm_ccw(vector <IVec> &p){
	int rSize;
	int cSize;

	rSize = p.size();
	cSize = p[0].size();

	vector<vector<int> > cw;

	cw.resize(cSize);

	for(int i = 0; i < cw.size(); i++){
		cw[i].resize(rSize);
	}

	int count = 0;
	int count2 = 0;

	for(int j = p[0].size() - 1; j >= 0; j--){
		for(int k = 0; k < p.size(); k++){
			cw[count2][count] = p[k][j];
			count++;
			if(count == cw[0].size()){
				count = 0;
				count2++;
			}
		}
	}

	p.resize(cSize);

	for(int l = 0; l < p.size(); l++){
		 p[l].resize(rSize);
	}

	for(int m = 0; m < p.size(); m++){
		for(int n = 0; n < p[0].size(); n++){
			p[m][n] = cw[m][n];
		}
	}

}

//This function pads out the vector with the width and pixel value given, meaning if the width was 10, there would be a width of 10 pixels on the left,
//right, bottom, and top of the pgm picture.
void pgm_pad(vector <IVec> &p, int w, int pv){
	int rSize = 0;
	int cSize = 0;

	rSize = p.size();
	cSize = p[0].size();

	p.resize(rSize + w*2);

	for(int i = 0; i < p.size(); i++){
		p[i].resize(cSize + w*2);
	}

	for(int j = rSize - 1; j >= 0; j--){
		for(int k = cSize - 1; k >= 0; k--){
			p[j+w][k+w] = p[j][k];
		}
	}
	
	for(int l = 0; l < w; l++){
		for(int m = 0; m < p[0].size(); m++){
			p[l][m] = pv;
			p[p.size() - 1 - l][m] = pv;
		}
	}
	
	for(int n = 0; n < p.size(); n++){
		for(int o = 0; o < w; o++){
			p[n][o] = pv;
			p[n][p[0].size() - 1 - o] = pv;
		}
	}
}

//This function makes panels of the given pgm by extending the pgm by the number of rows and columns that is given for the panelled pgm output.  If the given
//rows and columns were just 1, then it would be the original picture, but if there were 2 rows and 2 columns given, then there would be 2 of the original
//pgm going vertical and 2 of the original going out from each row.
void pgm_panel(vector <IVec> &p, int r, int c){
	int rSize = 0;
	int cSize = 0;
	
	rSize = p.size();
	cSize = p[0].size();

	p.resize(rSize * r);

	for(int i = 0; i < p.size(); i++){
		p[i].resize(cSize * c);
	}

	if(r*c > 1){
		for(int j = 0; j < p.size(); j++){
			for(int k = 0; k < p[j].size(); k++){
				p[j][k] = p[j%rSize][k%cSize];
			}
		}
	}

}

//This function crops the pgm file, starting at row index r and column index c (top left of picture), and the cropped picture will have a number of rows
//and columns given by the rows and cols value.
void pgm_crop(vector <IVec> &p, int r, int c, int rows, int cols){

	for(int i = 0; i <= r; i++ ){
		if( i == r ){            
			p.resize(rows);
		}
		else{
			p.erase(p.begin());
		}
	}
	
	for(int k = 0; k < rows; k++){
		for(int j = 0; j <= c; j++){
			if(j == c ){
				p[k].resize(cols);
			}
			else{                    
				p[k].erase(p[k].begin());
			}
		}
	}
}

// DO NOT CHANGE ANYTHING BELOW THIS LINE

void bad_pgm(string s)
{
  cerr << "Bad PGM file: " << s << endl;
  exit(1);

}

vector <IVec> pgm_read()
{
  string s;
  int r, c, i, j, v;
  vector <IVec> p;

  if (!(cin >> s)) bad_pgm("Empty file.");
  if (s != "P2") bad_pgm("First word is not P2.");
  if (!(cin >> c) || c <= 0) bad_pgm("Bad column spec.");
  if (!(cin >> r) || r <= 0) bad_pgm("Bad column spec.");
  if (!(cin >> i) || i != 255) bad_pgm("Bad spec of 255.");
  p.resize(r);
  for (i = 0; i < r; i++) for (j = 0; j < c; j++) {
    if (!(cin >> v) || v < 0 || v > 255) bad_pgm("Bad pixel.");
    p[i].push_back(v);
  }
  if (cin >> s) bad_pgm("Extra stuff at the end of the file.");
  return p;
}

void usage()
{
  cerr << "usage: pgm_editor command....\n\n";
  cerr << "        CREATE rows cols pixvalue\n";
  cerr << "        CW\n";
  cerr << "        CCW\n";
  cerr << "        PAD pixels pixvalue\n";
  cerr << "        PANEL r c\n";
  cerr << "        CROP r c rows cols\n";
  exit(1);
}

main(int argc, char **argv)
{
  istringstream ss;
  int r, c, i, j, p, w, rows, cols;
  vector <IVec> pgmf;
  string a1;

  if (argc < 2) usage();
  a1 = argv[1];

  if (a1 == "CREATE") {
    if (argc != 5) usage();
    ss.clear(); ss.str(argv[2]); if (!(ss >> r) || r <= 0) usage();
    ss.clear(); ss.str(argv[3]); if (!(ss >> c) || c <= 0) usage();
    ss.clear(); ss.str(argv[4]); if (!(ss >> p) || p < 0 || p > 255) usage();
    pgmf = pgm_create(r, c, p);
  } else if (a1 == "PAD") {
    if (argc != 4) usage();
    ss.clear(); ss.str(argv[2]); if (!(ss >> w) || w <= 0) usage();
    ss.clear(); ss.str(argv[3]); if (!(ss >> p) || p < 0 || p > 255) usage();
    pgmf = pgm_read();
    pgm_pad(pgmf, w, p);
  } else if (a1 == "CCW") {
    if (argc != 2) usage();
    pgmf = pgm_read();
    pgm_ccw(pgmf);
  } else if (a1 == "CW") {
    if (argc != 2) usage();
    pgmf = pgm_read();
    pgm_cw(pgmf);
  } else if (a1 == "PANEL") {
    if (argc != 4) usage();
    ss.clear(); ss.str(argv[2]); if (!(ss >> r) || r <= 0) usage();
    ss.clear(); ss.str(argv[3]); if (!(ss >> c) || c <= 0) usage();
    pgmf = pgm_read();
    pgm_panel(pgmf, r, c);
  } else if (a1 == "CROP") {
    if (argc != 6) usage();
    ss.clear(); ss.str(argv[2]); if (!(ss >> r) || r < 0) usage();
    ss.clear(); ss.str(argv[3]); if (!(ss >> c) || c < 0) usage();
    ss.clear(); ss.str(argv[4]); if (!(ss >> rows) || rows <= 0) usage();
    ss.clear(); ss.str(argv[5]); if (!(ss >> cols) || cols <= 0) usage();
    pgmf = pgm_read();
    if (r + rows > pgmf.size() || c + cols > pgmf[0].size()) {
      fprintf(stderr, "CROP - Bad params for the pictures size (r=%d, c=%d)\n",
           (int) pgmf.size(), (int) pgmf[0].size());
      exit(1);
    }
    pgm_crop(pgmf, r, c, rows, cols);
  } else {
    usage();
  }
  pgm_write(pgmf);
}
