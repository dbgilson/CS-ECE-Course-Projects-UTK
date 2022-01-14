//CS 302, Lab 4
//By: David Gilson
//The purpose of this code is to make a friends system where people are assigned friends.  The friends and friends-of-friends are then
//displayed by a doknow and mightknow text file.  This is Friendnet3, where we implement a vector of set<int> to properly order the index
//values of friends.  This uses sparse matrix methodology, but in a different way than friendnet2.  (Sets are already ordered and ensure
//uniqueness of indices.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <set>
using namespace std;

//This function takes in a name vector and the friends 2D vector and randomly assigns friends to the friends vector.
//A person can have a minimum of 1 friend and a max of 3 friends.  Since the 2D vector has a set in it instead of a vector,
//we just use insert() instead of push_back() to fill the set.
void set_oldfriends(vector <string> &name, vector<set<int> > &friends, int M0, int M1) {
	int N = (int)name.size();

	int c = 0;
	while(c < N){
		friends.push_back(set<int>() );
		c++;
	}

  for (int i=0; i<N; i++) {
	//declare std::set called doknow
	set<int> doknow;
	//initialize M (random number of friends: function of M0, M1)
	int M = M0 + rand() % M1;

	while ((int)doknow.size() < M) {
	  //compute j (random friend index: hint j != i)
	  int j;
	  while(true){
		  j = rand() % N;
		  if(j != i){
			  break;
		  }
	  }

	  doknow.insert(j);
	}
	
    set<int>::iterator it;
	for(it = doknow.begin(); it != doknow.end(); ++it){
		friends[i].insert(*it);
		friends[*it].insert(i);
	}
  }
}

//This is the function that determines the friends-of-friends and puts them in a new 2D vector.  This 2D vector also is a vector of set<int> to
//ensure uniqueness and sorted order.
void set_newfriends(vector<set<int> > &friends, vector<set<int> > &new_friends) {
  int N = (int)friends.size();

  int c = 0;
  while(c < N){
	  new_friends.push_back(set<int>() );
	  c++;
  }
  set<int>::iterator it1;
  set<int>::iterator it2;
  set<int>::iterator it3;
  int check = 0;
  for (int i=0; i<N; i++) {
	for(it1 = friends[i].begin(); it1 != friends[i].end(); ++it1){
	  for(it2 = friends[*it1].begin(); it2 != friends[*it1].end(); ++it2){
		  check = 0;
			for(it3 = friends[i].begin(); it3 != friends[i].end(); ++it3){
				if(*it3 == *it2){
					check = 1;
				}
			}
		  if(*it2 != i && check == 0){
			new_friends[i].insert(*it2);
			new_friends[*it2].insert(i);
		  }
	  }
    }
  }
}  

//This is the write function that writes a friends 2D vector to a file.
void writetofile(const char *fname, vector<string> &name, vector<set<int> > &friends) {
  //open file
  ofstream fout(fname);

  if(!(fout.is_open())){
	  cerr << "Error: File could not be open to write\n";
	  return;
  }

  int N = (int)name.size();
  unsigned int nsize = 0;
 // determine max name length
  for(int l = 0; l < N; l++){
	  if(name[l].length() > nsize){
		  nsize = name[l].size();
	  }
  }
  
  set<int>::iterator it;
  int count;
  for (int i=0; i<N; i++) {
	fout << setw(nsize) << left << name[i] << " :";
	count = 0;
	for(it = friends[i].begin(); it != friends[i].end(); ++it){
		if(count >= 8){
			fout << endl << setw(nsize) << left << name[i] << " :";
			count = 0;
		}
		fout << " " << setw(nsize) << left << name[*it];
		count++;
	}
	fout << endl;
  }
  //close file
  fout.close();
}

//In int main, we parse the command line to determine which friendnet we are using and what seed we are giving to srand.
//We then call the set_oldfriends function and set_newfriends function to fill the 2D vectors, and then we output them to
//doknow (direct friends) and mightknow (friend-of-friend) files.
int main(int argc, char *argv[]) {
  if(argc != 1 && argc != 3){
    	cerr << "usage: cat datafile.txt | Friendnet3 [-seed N]" << endl;
        return 0;
  }
  if(argc == 1){
	  srand(time(NULL));
  }
  else{
	  if(argc == 3 && strcmp(argv[1], "-seed") != 0){
			  cerr << "usage: cat datafile.txt | Friendnet3 [-seed N]" << endl;
			  return 0;
	  }
	  else{
		srand(atoi(argv[2]));
	  }
  }

  vector<string> name;
  string s;
  // read strings from stdin into name vector
  while(cin >> s){
	name.push_back(s);
  }

  int M0 = 1; // min number of friends
  int M1 = 3; // potential extra friends

  //Instead of vector<vector<int> >, we declare the Friends 2D vectors as vector<set<int> >
  vector<set<int> > friends;
  vector<set<int> > new_friends;

  set_oldfriends(name, friends, M0, M1);
  writetofile("doknow3.txt", name, friends);

  set_newfriends(friends, new_friends);
  writetofile("mightknow3.txt", name, new_friends);

  return 0;
}
