//CS140 Lab 6: Candy Crush, By David Gilson
//The purpose of this code is to make a simplified 1-row version of the popular game "Candy Crush" using lists.
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <fstream>
#include <sstream>
#include "candyCrush.h"

using namespace std;

//This is the constructor for the candy crush game.  It takes the inputs of the game seed and rowLength (row 1), the flavors used (row 2), and the probabilities of a row length and
//the points of that row length (row 3 to n).  This contains many error checking statements for the inputs.  This constructor pushes back values onto the variables, vectors, and
//candy list found in candyCrush.h .  The final product is a list of strings that contains rowLength number of elements of varying sequences and flavors.
candyCrush::candyCrush(string of){
	ifstream f;
	int seed, length, i1, i2, flavChosen, seqLength;
	istringstream ss;
	string s, s1, s2;
	int pr = 0;
	int rCount = 0;

	f.open(of.c_str());
	if(f.fail()){
		fprintf(stderr, "%s: No such file or directory\n", of.c_str());
		exit(1);
	}
	
	//Best to use getline due to the uncertainty of inputs.
	getline(f, s);
	ss.clear();
	ss.str(s);
	if(!(ss >> seed)){
		ss.clear();
		ss.str(s);
		ss >> s2;
		fprintf(stderr, "line 1: Bad seed for the random number generator\n");
		fprintf(stderr, "\terroneous input was: %s\n", s2.c_str());
		exit(1);
	}

	if(!(ss >> rowLength)){
		ss.clear();
		ss.str(s);
		ss >> s1 >> s2;
		fprintf(stderr, "line 1: Bad row length--must be an integer\n");
		fprintf(stderr, "\terroneous input was: %s\n", s2.c_str());
		exit(1);
	}

	if(rowLength <= 0 || rowLength > 100){
		fprintf(stderr, "line 1: The row length you entered, %d, must be from 1-100\n", rowLength);
		exit(1);
	}

	//Applying the seed.
	srand(seed);

	getline(f, s);
	ss.clear();
	ss.str(s);
	while(ss >> s1){
		flavors.push_back(s1);
	}
	int lcount = 3;
	while(getline(f, s)){
		ss.clear();
		ss.str(s);
		rCount++;
		if(!(ss >> i1)){
			ss.clear();
			ss.str(s);
			ss >> s1;
			fprintf(stderr, "line %d: Probability and points for this sequence must be non-negative integers\n", lcount);
			fprintf(stderr, "\terroneous input was: %s\n", s1.c_str());
			exit(1);
		}
		if(!(ss >> i2)){
            ss.clear();
            ss.str(s);
            ss >> s1 >> s2;
            fprintf(stderr, "line %d: Probability and points for this sequence must be non-negative integers\n", lcount);
            fprintf(stderr, "\terroneous input was: %s\n", s2.c_str());
            exit(1);
        }
		if(i1 > 100 || i1 < 0){
			fprintf(stderr, "Line %d: The probability you entered, %d, must be 0-100\n", lcount, i1);
			exit(1);
		}

        if(i2 < 0){
            fprintf(stderr, "Line %d: points, %d, is negative. It must be non-negative\n", lcount, i2);
            exit(1);
        }
			pr += i1;
			if(pr > 100){
				fprintf(stderr, "Line %d: The cumulative probability exceeds 100. It must be exactly 100 when all probabilities have been entered\n", lcount);
				exit(1);
			}
			probabilities.push_back(pr);
			points.push_back(i2);
		lcount++;
	}
	
	if(pr != 100){
		fprintf(stderr, "The probabilities you entered must sum to 100 but their sum was %d\n", pr);
		exit(1);
	}

	if(rCount != rowLength){
		fprintf(stderr, "You entered %d pairs of probabilities and points. You must enter the same number of pairs as the rowLength, which is %d\n", rCount, rowLength);
		exit(1);
	}

	//This portion calculates the random flavor and sequence length to add to the candy list.  It stops once the list is full.
	int test = 0;
	while(test == 0){
		flavChosen = (rand()%(flavors.size()));
		
		seqLength = (rand()%100)+1;

		int k = 0;
		int t = 0;
		for(int i = 0; i < probabilities.size(); i++){
			if((seqLength <= probabilities.at(i)) && (t == 0)){
				k = i;
				t = 1;
			}
		}

		int count = 1;
		int test2 = 0;
		while(test2 == 0){
			candy.push_back(flavors.at(flavChosen));
			if(candy.size() == rowLength){
				test = 1;
				test2 = 1;
			}
			if(count == (k+1)){
				test2 = 1;
			}
			count++;
		}
	}
	f.close();
}

//This function simply returns the row length of the candy list.
int candyCrush::getRowLength(){
	return rowLength;
}

//This function simply returns the current stored score.
int candyCrush::getScore(){
	return score;
}

//This function prints off the candy list.  It should print 8 to a row, with each element given 10 spaces, left justified.
void candyCrush::printCandy(){
	list <string>::iterator lit;
	
	int count = 1;
	for(lit = candy.begin(); lit != candy.end(); lit++){
		if(count != 8){
			printf("%-10s", (*lit).c_str());
			count++;
		}
		else{
			printf("%-10s\n", (*lit).c_str());
			count = 1;
		}
	}
	if(count != 1){
		printf("\n");
	}

}

//This function allows the user to "play" the game.  The user inputs a choice, and based on that choice (index), the function determines the location of the choice on the list, 
//determines the flavor, determines the amount of flavors in sequence around the given index, deletes that sequence, and adds new sequences to the end of the list.  This function
//actually returns the score that was determined from the sequence length.
int candyCrush::play(int choice){
	list <string>::iterator lit;
	list <string>::reverse_iterator lit2;
	list <string>::iterator temp;
	list <string>::reverse_iterator temp2;
	int temps;
	int fLength = 0;

	int ind = 0;
	string f;

	//I actually used both a regular and reverse iterator for a later portion of the function.  This point just determines what flavor the index is.
	for(lit = candy.begin(); lit != candy.end(); lit++){
		if(ind == choice){
			f = *lit;
			temp = lit;
			ind++;
		}
		else{
			ind++;
		}
	}
	
	ind = candy.size() - 1;

	for(lit2 = candy.rbegin(); lit2 != candy.rend(); lit2++){
		if(ind == choice){
			temp2 = lit2;
			ind--;
		}
		else{
			ind--;
		}
	}		

	//I decided to change the index and the flavor sequence to something that I could used candy.remove() on, so I had to use both a regular and reverse iterator if I wanted to 
	//cover the entire list.
	for(lit = temp; lit != candy.end(); lit++){
		if(*lit == f){
			fLength++;
			*lit = "Delete Me";
		}
		else{
			break;
		}
	}

	for(lit2 = temp2; lit2 != candy.rend(); lit2++){
		if(*lit2 == f || *lit2 == "Delete Me"){
			fLength++;
			*lit2 = "Delete Me";
		}
		else{
			break;
		}
	}

	fLength = fLength - 2;
	score += points.at(fLength);
	temps = points.at(fLength);
	
	candy.remove("Delete Me");
	
	//This portion is the same as in the constructor when determining what to add onto the list until it is full.
	int flavChosen, seqLength;
	int test = 0;
    while(test == 0){
        flavChosen = rand()%(flavors.size());

        seqLength = (rand()%100)+1;

        int k = 0;
		int t = 0;
        for(int i = 0; i < probabilities.size(); i++){
            if((seqLength <= probabilities.at(i)) && (t == 0)){
                k = i;
				t = 1;
            }
        }

        int count = 1;
        int test2 = 0;
        while(test2 == 0){
            candy.push_back(flavors.at(flavChosen));
            if(candy.size() == rowLength){
                test = 1;
                test2 = 1;
            }
            if(count == (k+1)){
                test2 = 1;
            }
            count++;
        }
    }
	return temps;
}


