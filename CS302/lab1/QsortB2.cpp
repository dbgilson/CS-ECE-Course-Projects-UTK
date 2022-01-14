//CS302 Lab1 B
//By: David Gilson
//The purpose of this code is to take in a .txt file with format firstname, lastname, and phonenumber and rearrange the list in sorted order to
//print to stdout.  This part includes both the code and comments of part A, as well as the code and comments of part B. Part B of the lab was
//to include the quicksort and quickselect functions, as well as give command line options and checks.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

//This is the data class that will hold the firstname, lastname, and phonumber of the input data we are given.
class data {
  public:
	//We overloaded the operator< in order to actually compare data objects directly.  (I found out that you can just "return this->firstname < r.firstname", but
	//this approach helps me visualize it better.)
	bool operator<(const data &r) const{
		if(this->lastname == r.lastname){
			if(this->firstname == r.firstname){
				if(this->phonenum < r.phonenum){
					return true;
				}
				else{
					return false;
				}
			}
		
			if(this->firstname < r.firstname){
				return true;
			}
			else{
				return false;
			}
		}
		if(this->lastname < r.lastname){
			return true;
		}
		else{
			return false;
		}
	}

    friend istream & operator>>(istream &, data &);
    friend ostream & operator<<(ostream &, const data &);

  private:
    string firstname;
    string lastname;
    string phonenum;
};

//We wrote this input operator in order to direclty input the information into the data class.
istream & operator>>(istream &in, data &r) { 
	in >> r.firstname >> r.lastname >> r.phonenum;
	return in;
}

//This output operator allows us to cout the data directly, already formatted as in the solution code.  (I pretty much just changed the 23 number around until it was the same
//output as the solution code, which I checked using the diff function.)
ostream & operator<<(ostream &out, const data &r) {
	int l = 23 - r.lastname.length();
	out << left << r.lastname << " " << setw(l) << left << r.firstname << r.phonenum;
	return out;
}

//Here is the function to call and print the data vector found in int main.  It takes vector iterators in order to go through the list, and I have to use a temporary iterator 
//since the iterators given are const.  The list was already formatted by the operator<<, so I can just cout whatever is given.
template <typename T>
void printlist(const T & begin, const T & end) { 
	vector<data>::iterator temp = begin;
	while(1){
		if (temp == end){
			break;
		}
		cout << *temp << endl;
		temp++;
	}
}

////////////////////////////////////////////////////
//Lab1 B Material

//Here is the quicksort function, which has both the random partitioning code and the recursive quicksort calling algorithm.
template<typename Tdata>
void quicksort(vector<Tdata> &A, int left, int right){
	//Start with the quicksort looping statement
	if(left < right){
		//Add our random pivot partitioning
		int n = rand();
		int pv = n%(right-left+1) + left;
		Tdata pivot = A[pv];

		swap(A[right], A[pv]);
		int i = left-1;
		int j = right;

		while (1) {
			while(A[++i] < pivot){ }
			while (pivot < A[--j]){
				//Important Bound Check
				if(j == left) break;
			}
				if(i >= j) break;
			swap(A[i], A[j]);
		}
		
		pv = i;
		swap(A[right], A[pv]);

		//Call the quicksorts recursively with changed lefts or rights.
		quicksort(A, left, pv-1);
		quicksort(A, pv+1, right);
	}
}

//Here is the quickselect function, which takes in 4 values (data, left, search value, right) and quickly goes through
//the given data in the domain we gave it in order to partition it for the quicksort function.
template<typename Tdata>
void quickselect(vector<Tdata> &A, int left, int k, int right){ 
	//Start of while loop
	while(left < right){
		//Partitioning From QuickSort code
		int n = rand();
		int pv = n%(right-left+1) + left;
		Tdata pivot = A[pv];

		swap(A[right], A[pv]);
		int i = left -1;
		int j = right;

		while(1){
			while(A[++i] < pivot){ }

			while(pivot < A[--j]) {
				if(j == left){
					break;
				}
			}
			if (i >= j){
				break;
			}
			swap(A[i], A[j]);
		}
		
		pv = i;
		swap(A[right], A[pv]);
		
		//Quickselect Checking Given to us
		if (k == pv){
			return;
		}
		if (k < pv){
			right = pv-1;
		}
		else{
			left = pv+1;
		}
	}
}

int main(int argc, char *argv[]) {
	//This code is the command line checking.  It checks for "-stl", "-rpivot", if you have the right
	//number of inputs based on which one you chose, and if k0 and k1 are integers.
	if(argc < 2){
		cerr << "usage: ./QsortB -stl | -rpivot [k0 k1] < file.txt" << endl;
		return 0;
	}
	if(strcmp(argv[1], "-stl") && strcmp(argv[1], "-rpivot")){
		cerr << "usage: ./QsortB -stl | -rpivot [k0 k1] < file.txt" << endl;
		return 0;
	}
	if(!strcmp(argv[1], "-rpivot")){
		if(argc == 3){
			cerr << "usage: ./QsortB -stl | -rpivot [k0 k1] < file.txt" << endl;
			return 0;
		}
		if(argc == 4){
			if(!atoi(argv[2]) || !atoi(argv[3])){
				cerr << "usage: ./QsortB -stl | -rpivot [k0 k1] < file.txt" << endl;
				return 0;
			}
		}
	}
	if(!strcmp(argv[1], "-stl")){
		if(argc > 2){
			cerr << "usage: ./QsortB -stl | -rpivot [k0 k1] < file.txt" << endl;
			return 0;
		}
	}

  //Data vector creation and read in.
  vector<data> A;

  data din;
  while (cin >> din)
    A.push_back(din);

  //If -stl was called, use given sort function.
  if(!strcmp(argv[1], "-stl")){
	std::sort(A.begin(), A.end());
  }
  //If -rpivot was called, check for changes in k0 and k1, update, and call quickselect for the given values.
  else if(!strcmp(argv[1], "-rpivot")){
		  
	int N = (int)A.size();
	
	int k0 = 0;
	int k1 = N-1;

	if(argc == 4){
		k0 = atoi(argv[2]);
		k1 = atoi(argv[3]);
		if(k0 < 0){
			k0 = 0;
		}
		if(k1 > N-1){
			k1 = N - 1;
		}
		quickselect(A, 0, k0, N-1);
		quickselect(A, k0, k1, N-1);
	}
	//Call quicksort, regardless of updated k0/k1 or not since -rpivot was called.
	quicksort(A, k0, k1);
  }
  printlist(A.begin(), A.end());
}
