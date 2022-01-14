//CS140 Lab 4 By David Gilson
//The purpose of this code is to be able to add hash values to a hash table, find values in the hash table, and be able to print them out.
#include "hash140.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <vector>

using namespace std;

//This is just the constructor for the hash table.  It makes sure values are initialized and that the size of the table is right.
HashTable::HashTable(int table_size, string function, string collision){

	for(int i = 0; i < table_size; i++){
		keys.push_back("");
		vals.push_back("");
	}

	nkeys = 0;

	if(function == "Last7"){
		Fxn = 1;
	}
	if(function == "XOR"){
		Fxn = 2;
	}

	if(collision == "Linear"){
		Coll = 1;
	}
	if(collision == "Double"){
		Coll = 2;
	}
};

//This function, along with Find, is the majority of the code.  It looks for which hash function and collision resolution was made with the table, and 
//it takes the given key through the selected function and collision resolution made in the constructor.  If the function finds an empty string in the found
//index, the it inserts the key at the given index value in the key vector and the correlating value in the value vector with the given index.
void HashTable::Add_Hash(string &key, string &val){
	unsigned int i, Finali;
	unsigned int hash = 0, index = 0;
	int ksize, tSize;
	int count = 0;
	istringstream ss;
	string s, L7;
	vector <string> XOR, XOR2;
	vector <int> XORi, XORi2;

	if (keys.size() == nkeys){
		fprintf(stderr, "Hash Table Full\n");
		exit(1);
	}

	ksize = key.size();
	tSize = keys.size();

	//If Last7
	if(Fxn == 1){
		if(ksize < 7){
			ss.clear();
			ss.str(key);
			ss >> hex >> Finali;
		}

		else{
			L7 = key.substr(ksize - 7, 7);
			ss.clear();
			ss.str(L7);
			ss >> hex >> Finali;
		}
		
		index = Finali % tSize;
	}
	
	//If XOR
	else if(Fxn == 2){
		for(int j = 0; j < ksize; j += 7){
			XOR.push_back(key.substr(j, 7));
		}

		for(int l = 0; l < XOR.size(); l++){
			ss.clear();
			ss.str(XOR.at(l));
			ss >> hex >> i;
			XORi.push_back(i);
		}

		for(int m = 0; m < XORi.size(); m++){
			hash = hash ^ XORi.at(m);
		}

		Finali = hash;
		
		index = Finali % tSize;
	}

	//If Linear
	if(Coll == 1){
		int test = 1;
		int iCount = 0;

		while(test > 0){
			if(vals[index] == ""){
				keys[index] = key;
				vals[index] = val;
				nkeys++;
				test = -1;
			}
			else{
				iCount++;
				if(iCount == tSize){
					fprintf(stderr, "Hash Table Full\n");
					exit(1);
				}
				index++;
				index = index % tSize;
			}
		}
	}
	
	//If Double
	else if(Coll == 2){
		int test2 = 1;
		int iCount2 = 1;
		unsigned int oIndex = index;

		while(test2 > 0){
			if(vals.at(index) == ""){
				keys.at(index) = key;
				vals.at(index) = val;
				nkeys++;
				test2 = -1;
			}
			else{
				//If Last7 was given, use XOR as the second hash function.
				if(Fxn == 1){
					if(iCount2 >= tSize){
                        fprintf(stderr, "Couldn't put %s into the table\n", key.c_str());
						exit(1);
                    }

					unsigned int index2 = 0;
					unsigned int Finali2 = 0;
					hash = 0;

					for(int n = 0; n < ksize; n += 7){
						XOR2.push_back(key.substr(n, 7));
					}
					for (int o = 0; o < XOR2.size(); o++){
						ss.clear();
						ss.str(XOR2.at(o));
						ss >> hex >> i;
						XORi2.push_back(i);
				    }

					for(int m = 0; m < XORi2.size(); m++){
						hash = hash ^ XORi2.at(m);
			        }

					Finali2 = hash;
					index2 = Finali2 % tSize;
					if(index2 == 0){
						index2 = 1;
					}

					index = (oIndex + iCount2*(index2)) % tSize;
					iCount2++;
				}
				
				//If XOR was given, use Last7 as the second hash function.
				else if(Fxn == 2){
					if(iCount2 >= tSize){
                        fprintf(stderr, "Couldn't put %s into the table\n", key.c_str());
                        exit(1);
                    }

					unsigned int index2 = 0;
					unsigned int Finali2 = 0;
					string L72;

					if(ksize < 7){
						ss.clear();
            			ss.str(key);
            			ss >> hex >> Finali2;
        			}

        			else{
            			L72 = key.substr(key.size() - 7, 7);
            			ss.clear();
            			ss.str(L72);
            			ss >> hex >> Finali2;
					}

        			index2 = Finali2 % tSize;
					if(index2 == 0){
						index2 = 1;
					}

					index = (oIndex + iCount2*(index2)) % tSize;
					iCount2++;
				}
			}

		}
	}
};

//Find is very similar to Add in structure, but during the collision resolution section, instead of finding an empty val to put the values in, we see if 
//the index made from the key given contains the key given, and if it doesn't, then it goes through the resolutions until it either finds it or goes through
//the table enough to know it's not there.
string HashTable::Find(string &key){
	unsigned int i, Finali;
    unsigned int hash = 0, index = 0;
    int ksize, tSize;
    int count = 0;
    istringstream ss;
    string s, L7;
    vector <string> XOR, XOR2;
    vector <int> XORi, XORi2;

    ksize = key.size();
	tSize = keys.size();

    //If Last7
    if(Fxn == 1){
        if(ksize < 7){
            ss.clear();
            ss.str(key);
            ss >> hex >> Finali;
        }

        else{
            L7 = key.substr(ksize - 7, 7);
            ss.clear();
            ss.str(L7);
            ss >> hex >> Finali;
        }
		
        index = Finali % tSize;
    }

	//If XOR
    else if(Fxn == 2){
        for(int j = 0; j < ksize; j += 7){
            XOR.push_back(key.substr(j, 7));
        }

        for(int l = 0; l < XOR.size(); l++){
            ss.clear();
            ss.str(XOR.at(l));
            ss >> hex >> i;
            XORi.push_back(i);
        }

        for(int m = 0; m < XORi.size(); m++){
            hash = hash ^ XORi.at(m);
        }

        Finali = hash;

        index = Finali % tSize;
    }

    //If Linear
    if(Coll == 1){
        int test = 0;
        int iCount = 0;

        while(test == 0){
            if(keys.at(index) == key){	
                nkeys++;
                test = -1;
				return vals.at(index);
            }
            else{
                iCount++;
                if(iCount == tSize){
					tmp = 0;
                    return "";
                }
                index++;
				index = index % tSize;

				tmp++;
            }
		}
	}

    //If Double
    else if(Coll == 2){
        int test2 = 0;
        int iCount2 = 1;
        unsigned int oIndex = index;

        while(test2 == 0){
            if(keys[index] == key){
                nkeys++;
                test2 = -1;
				return vals[index];
            }
            else{
				//If Last7 was given, use XOR as the second hash function
                if(Fxn == 1){
					if(iCount2 >= tSize){
						return "";
						tmp = 0;
					}

                    unsigned int index2 = 0;
                    unsigned int Finali2 = 0;
                    hash = 0;

                    for(int n = 0; n < ksize; n += 7){
                        XOR2.push_back(key.substr(n, 7));
                    }
                    for (int o = 0; o < XOR2.size(); o++){
                        ss.clear();
                        ss.str(XOR2.at(o));
                        ss >> hex >> i;
                        XORi2.push_back(i);
                    }

					for(int m = 0; m < XORi2.size(); m++){
                        hash = hash ^ XORi2.at(m);
                    }

                    Finali2 = hash;
                    index2 = Finali2 % tSize;
					if(index2 == 0){
						index2 = 1;
					}

                    index = (oIndex + iCount2*(index2)) % tSize;
					iCount2++;
					tmp++;
				}
				
				//If XOR was given, use Last7 as the second hash function.
                else if(Fxn == 2){
                    unsigned int index2 = 0;
                    unsigned int Finali2 = 0;
                    string L72;
					
					if(iCount2 >= tSize){
						return "";
						tmp = 0;
					}

                    if(ksize < 7){
                        ss.clear();
                        ss.str(key);
                        ss >> hex >> Finali2;
                    }

                    else{
                        L72 = key.substr(key.size() - 7, 7);
                        ss.clear();
                        ss.str(L72);
                        ss >> hex >> Finali2;
                    }

                    index2 = Finali2 % tSize;
					if(index2 == 0){
						index2 = 1;
					}
                    index = (oIndex + iCount2*(index2)) % tSize;
					iCount2++;
					tmp++;
                }
            }

		}
	}
};

//This function just prints the keys and correlating values of the hash table that aren't empty strings.
void HashTable::Print(){
	for(int i = 0; i < vals.size(); i++){
		if(vals[i] != ""){
			printf("%5d %s %s\n", i, keys[i].c_str(), vals[i].c_str());
		}
	}
};

//This function determines the total number of probes it takes to find all of the values in the given hash table.  If Find finds the key given the first time,
//then tmp is 0; if Find finds the key the second time (in the collision resolution), then tmp is 1, and so on.  tmp is stacked with each key that is in the
//table, totaling up to the total number of probes made.
int HashTable::Total_Probes(){
	string fTemp;
	int tmp2 = 0;
	tmp = 0;

	for(int i = 0; i < keys.size(); i++){
		if(keys[i] != ""){
			fTemp = Find(keys[i]);
			tmp2 += tmp;
			tmp = 0;
		}
	}

	return tmp2;
};
