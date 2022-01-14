//CS 302, Lab 4
//By: David Gilson
//The purpose of this code is to make a friends system where people are assigned friends.  The friends and friends-of-friends are then
//displayed by a doknow and mightknow text file.  This is Friendnet1, where the friend system uses vector of vectors to keep the
//friend index in place.  This version uses dense matrix methodology, where the 2D vector is a symmetric matrix.
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
//A person can have a minimum of 1 friend and a max of 3 friends.  The 2D vector stores a 1 if there is a friend at
//that index, and 0 otherwise.
void set_oldfriends(vector <string> &name, vector<vector<int> > &friends, int M0, int M1) {
	int N = (int)name.size();
  //initialize 2D-vector-array friends (hint: use vector::assign())
	friends.assign(N, vector<int>(N, 0));

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
		friends[i][*it] = 1;
		friends[*it][i] = 1;
	}
  }
}

//This is the function that determines the friends-of-friends and puts them in a new 2D vector, storing a 1 at an index if there 
//is a friend-of-friend and storing a 0 otherwise.
void set_newfriends(vector<vector<int> > &friends, vector<vector<int> > &new_friends) {
  int N = (int)friends.size();
  new_friends.assign(N, vector<int>(N, 0));

  for (int i=0; i<N; i++) {
	for(int j = 0; j < N; j++){
	  if(friends[i][j] == 1){
	  for(int k = 0; k < N; k++){
		if(friends[j][k] == 1){
		  if(k != i && friends[i][k] == 0){
			new_friends[i][k] = 1;
			new_friends[k][i] = 1;
		  }
		}
	  }
	  }
	}
  }
}

//This is the write function that writes out a friends 2D vector to a file.
void writetofile(const char *fname, vector<string> &name, vector<vector<int> > &friends) {
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

  int count = 0;
  for (int i=0; i<N; i++) {
	//for (each adjacent friend: friends[i][j] == 1) {
	fout << setw(nsize) << left << name[i] << " :";
	count = 0;
	for(int j = 0; j < N; j++){
		if(friends[i][j] == 1 || friends[j][i] == 1){
			if(count >= 8){
				fout << endl << setw(nsize) << left << name[i] << " :";
				count = 0;
			}
			fout << " " << setw(nsize) << left << name[j];
			count++;
		}
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
    	cerr << "usage: cat datafile.txt | Friendnet1 [-seed N]" << endl;
        return 0;
  }
  if(argc == 1){
	  srand(time(NULL));
  }
  else{
	  if(argc == 3 && strcmp(argv[1], "-seed") != 0){
			  cerr << "usage: cat datafile.txt | Friendnet1 [-seed N]" << endl;
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

  vector<vector<int> > friends;
  vector<vector<int> > new_friends;

  set_oldfriends(name, friends, M0, M1);
  writetofile("doknow1.txt", name, friends);

  set_newfriends(friends, new_friends);
  writetofile("mightknow1.txt", name, new_friends);

  return 0;
}
