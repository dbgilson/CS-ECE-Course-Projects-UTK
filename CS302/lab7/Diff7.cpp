//CS302 Lab 7: Diff
//By: David Gilson
//The point of this code is to implement the unix command "diff" with 2 files.  We compare the lines (not individual characters) in order to tell a difference in the 
//files.  We can then either insert from a modified file, delete from the original file, or a combination of both.  This diff implementation uses the 
//longest common sequence (LCS) algorithm in order to determine a backtracing order.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cmath>
#include <stack>
#include <climits>

using namespace std;

//Here is the matrix class from when we did lab 3
template <typename T>
class matrix {
 	public:
 		matrix(int n_Nrows, int n_Ncols) {
 			Nrows = n_Nrows;
 			Ncols = n_Ncols;
 			buf = new T [Nrows*Ncols];
 		}
 		~matrix() {
 			delete [] buf;
 		}
 		int get_Nrows() const { return Nrows; }
 		int get_Ncols() const { return Ncols; }
 		T * operator[] (int i) { return &buf[i*Ncols]; }
 	private:
 		int Nrows, Ncols;
 		T *buf;
};

//Here is the LCS class, where we take 2 files (through 2 vectors of strings) and compute their alignment.  There are plenty of helper functions to 
//compute and print out the cost and link matrices created, as well as function wrappers.
class LCS {
  public:
    void text1_push_back(string);
    void text2_push_back(string);
	void compute_alignment();
	void report_difference();
	void print_cost();
	void print_link();
	void rp_difference();
	void print_sub(int, int, int, int);

  private:
	// support functions
	void compute_alignment1(vector<string>&, vector<string>&);
	void report_difference(int, int);
    // text1 buffer
	vector<string> text1;
    // text2 buffer
	vector<string> text2;

	//DP align 
	const int VERT = 1;
	const int HORZ = 2;
	const int DIAG = 4;

	const int DEL = 1;
	const int INS = 1;
	const int SUB = 0;
	matrix<int> *cost;
	matrix<int> *link;
	stack<int> t;
	vector<int> trace;
};

//The next 2 functions simply pushback each line of the files to 2 different text buffers.
void LCS::text1_push_back(string S){
	text1.push_back(S);
}

void LCS::text2_push_back(string S){
	text2.push_back(S);
}

//This function computes the alignment of the two files by filling in the cost and link matrices, where the
//DEL, INS, SUB, VERT, HORZ, and DIAG costs are all made in the class declaration.
void LCS::compute_alignment1(vector<string> &t1, vector<string> &t2){
	int i = t1.size();
	int j = t2.size();

	cost = new matrix<int>(i+1, j+1);
	link = new matrix<int>(i+1, j+1);

	(*cost)[0][0] = 0;
	(*link)[0][0] = 0;

	for(int x = 1; x <= i; x++){
		(*cost)[x][0] = (*cost)[x - 1][0] + DEL;
		(*link)[x][0] = VERT;
	}
	for(int y = 1; y <= j; y++){
		(*cost)[0][y] = (*cost)[0][y - 1] + INS;
		(*link)[0][y] = HORZ;
	}

    for(int x = 1; x <= i; x++){
		for(int y = 1; y <= j; y++){ 
			if(t1[x - 1] == t2[y - 1]){
				(*cost)[x][y] = (*cost)[x - 1][y - 1];
				(*link)[x][y] = DIAG; //where diag == 4
			}
			else{
				(*cost)[x][y] = INT_MAX;
			}

			int delcost = (*cost)[x - 1][y] + 1;
			int inscost = (*cost)[x][y - 1] + 1;
			
			if(inscost < (*cost)[x][y]){
				(*cost)[x][y] = inscost;
				(*link)[x][y] = HORZ;
			}
			if(delcost < (*cost)[x][y]){
				(*cost)[x][y] = delcost;
				(*link)[x][y] = VERT;
			}
		}
	}
}

//This is just a wrapper function to call the private compute_alignment
void LCS::compute_alignment(){
    this->compute_alignment1(text1, text2);
}

//This is just a wrapper function to call the private report_difference
void LCS::report_difference(){
	int m = text1.size();
	int n = text2.size();
	report_difference(m, n); //Where m,n are the bottom right corner
}

//This function creates the backtrace stack that we will later use to fill our trace vector
void LCS::report_difference(int i, int j){
	if(i == 0 && j == 0){
		return;
	}
	int link_ij;
	link_ij = (*link)[i][j];

	if(link_ij == DIAG){
		t.push(4);
		report_difference(i-1, j-1);
	}
	if(link_ij == VERT){
		t.push(1);
		report_difference(i-1, j);
	}
	if(link_ij == HORZ){
		t.push(2);
		report_difference(i, j-1);
	}
}

//This function takes our trace vector and calls a print function based on when a match is found (or at the end of the files)
void LCS::rp_difference(){
	int temp;
	while(!t.empty()){
		temp = t.top();
		t.pop();
		trace.push_back(temp);
	}
	int x = 0;
	int y = 0;
	int x_dist = 0;
	int y_dist = 0;
	for(unsigned int i = 0; i < trace.size(); i++){
		switch(trace[i]){
			case 2: //INSERT
				x_dist++;
				break;
			case 1: //DELETE
				y_dist++;
				break;
			case 4: //MATCH/SUB
				this->print_sub(x, y, x_dist, y_dist);
				x += x_dist + 1;
				y += y_dist + 1;
				x_dist = 0;
				y_dist = 0;
				break;
			default:
				break;
		}
	}
	this->print_sub(x, y, x_dist, y_dist);
	delete cost;
	delete link;
}

//This is the helper function that actually prints out the differences when called by rp_difference
void LCS::print_sub(int x, int y, int x_dist, int y_dist){
	int x_temp = 0;
	int y_temp = 0;

	//SUB -- c
    if(x_dist != 0 && y_dist != 0){
		x_temp = x + x_dist;
		y_temp = y + y_dist;
		if(x_dist == 1 && y_dist == 1){
			cout << (y+1) << "c" << (x+1) << endl;
			cout << "< " << text1[y] << endl;
			cout << "---" << endl;
			cout << "> " << text2[x] << endl;

		}
		else if(x_dist == 1 && y_dist != 1){
			cout << (y+1) << "," << y_temp << "c" << (x+1) << endl;
			for(int i = y; i < y_temp; i++){
				cout << "< " << text1[i] << endl;
			}
			cout << "---" << endl;
			cout << "> " << text2[x] << endl;
		}
		else if(x_dist != 1 && y_dist == 1){
			cout << (y+1) << "c" << (x+1) << "," << x_temp << endl;
			cout << "< " << text1[y] << endl;
			cout << "---" << endl;
			for(int i = x; i < x_temp; i++){
				cout << "> " << text2[i] << endl;
			}
		}
		else{
			cout << (y + 1) << "," << y_temp << "c" << (x+1) << "," << x_temp << endl;
			for(int i = y; i < y_temp; i++){
				cout << "< " << text1[i] << endl;
			}
			cout << "---" << endl;
			for(int i = x; i < x_temp; i++){
				cout << "> " << text2[i] << endl;
			}
		}
	}
	
	//INSERT ONLY -- a
	if(x_dist != 0 && y_dist == 0){
		x_temp = x + x_dist;
		if(x_dist == 1){
			cout << y << "a" << (x+1) << endl;
			cout << "> " << text2[x] << endl;
		}
		else{
			cout << y << "a" << (x+1) << "," << x_temp << endl;
			for(int i = x; i < x_temp; i++){
				cout << "> " << text2[i] << endl;
			}
		}
	}
	
	//DELETE ONLY -- d
	if(x_dist == 0 && y_dist != 0){
		y_temp = y + y_dist;
		if(y_dist == 1){
			cout << (y+1) << "d" << x << endl;
			cout << "< " << text1[y] << endl;
		}
		else{
			cout << (y+1) << "," << y_temp << "d" << x << endl;
			for(int i = y; i < y_temp; i++){
				cout << "< " << text1[i] << endl;
			}
		}
	}
}

//These next 2 functions just print out the cost and link matrices in order to view them (for debugging)
void LCS::print_cost(){
	for(int i = 0; i < cost->get_Nrows(); i++){
		for(int j = 0; j < cost->get_Ncols(); j++){
			cout << setw(2) << left << (*cost)[i][j] << "  ";
		}
		cout << endl;
	}
}

void LCS::print_link(){
	for(int i = 0; i < link->get_Nrows(); i++){
		for(int j = 0; j < link->get_Ncols(); j++){
			cout << setw(2) << left << (*link)[i][j] << "  ";
		}
		cout << endl;
	}

}
int main(int argc, char *argv[]){
  // check two input files are specified on command line
  if(argc != 3){
	  cout << "usage: ./Diff7 original.txt modified.txt" << endl;
	  return 0;
  }
  LCS lcs;  // instantiate your "LCS based diff" object

  // read the text from file1 into the lcs.text1 buffer
  string S;
  ifstream fin;
  fin.open(argv[1]);
  if(!fin.is_open()){
  	  cout << "Error in opening file 1" << endl;
	  return 0;
  }
  while(getline(fin, S)){
	  lcs.text1_push_back(S);
  }

  fin.close();

  // read the text from file2 into the lcs.text2 buffer
  fin.open(argv[2]);
  if(!fin.is_open()){
	  cout << "Error in opening file 2" << endl;
	  return 0;
  }
  while(getline(fin, S)){
	  lcs.text2_push_back(S);
  }

  lcs.compute_alignment();
  //If you wanted to see the cost and link matrices, you can add the 3 lines below
  //lcs.print_cost();
  //cout << endl;
  //lcs.print_link();

  lcs.report_difference();
  lcs.rp_difference();
}
