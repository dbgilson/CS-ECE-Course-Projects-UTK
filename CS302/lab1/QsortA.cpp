//CS302 Lab1
//By: David Gilson
//The purpose of this code is to take in a .txt file with format firstname, lastname, and phonenumber and rearrange the list in sorted order to
//print to stdout.  This is just part 1 of the lab, as QsortB will actually implement our own Quicksort algorithm code.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
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

//In here, a vector of type data is made; the input is given to fill the vector; the vector is sorted by the std sort function; and we output it using our printlist function.
int main(int argc, char *argv[]) {
  vector<data> A;

  data din;
  while (cin >> din)
    A.push_back(din);
  
  std::sort(A.begin(), A.end());

  printlist(A.begin(), A.end());
}
