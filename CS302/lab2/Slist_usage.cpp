//CS302 Lab 2
//By: David Gilson
//This is the main file of the Slist_usage program.  It is meant to take an input for the data class, fit the data into a Slist vector, and sort the data using 
//smart pointer algorithms.  Most of this file is similar to Lab 1 part B, as we are still reading in data (firstname, lastname, phonenum).  In the output, each person
//now has an ID number given to them as well to reflect the order of appearance.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include "Slist.h"
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
	int ID;
};

//We wrote this input operator in order to direclty input the information into the data class.
istream & operator>>(istream &in, data &r) { 
	in >> r.firstname >> r.lastname >> r.phonenum;
	static int i;
	r.ID = ++i;
	return in;
}

//This output operator allows us to cout the data directly, already formatted as in the solution code.  The only difference in the output compared to lab 1 is the addition
//of the ID at the end. (and a weird space at the beginning)
ostream & operator<<(ostream &out, const data &r) {
	int l = 23 - r.lastname.length();
	out << " " << left << r.lastname << " " << setw(l) << left << r.firstname << r.phonenum << setw(9) << right << r.ID;
	return out;
}

//Here is the function to call and print the data vector found in int main.  It takes vector iterators in order to go through the list, and I have to use a temporary iterator 
//since the iterators given are const.  This function was changed in this lab to account for the Slist vectors, so we used the Slist iterators and ++ methods given to us
//in the Slist header file.
template <typename T>
void printlist(const T & begin, const T & end) { 
	slist<data>::iterator temp = begin;
	while(temp != end){
		cout << *temp << endl;
		++temp;
	}
}

int main(int argc, char *argv[]) {
	//This code is the command line checking.  It checks for "-quicksort" and "-mergesort" and if you have the right
	//number of inputs.
	if(argc < 2 || argc >= 3){
		cerr << "usage: ./Slist_usage -quicksort|mergesort < file.txt" << endl;
		return 0;
	}
	if(strcmp(argv[1], "-quicksort") && strcmp(argv[1], "-mergesort")){
		cerr << "usage: ./Slist_usage -quicksort|mergesort < file.txt" << endl;
		return 0;
	}
  string algname = argv[1];
  
  //Custom vector of Slist
  slist<data> A;

  data din;
  while (cin >> din)
    A.push_back(din);

  //Call Slist sort function, then print out the sorted data.
  A.sort(algname);

  printlist(A.begin(), A.end());
}
