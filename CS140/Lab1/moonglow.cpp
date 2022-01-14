// CS140 Lab1 Moonglow Section By: David Gilson
// This code is meant to take a txt file of words specified by the lab and read them to pick out certain aspects and numbers to form a final answer.
// This code consists of a lot of flags to see what word is read or what to do with a number that has been read.

//First are the libraries and using namespace
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <cstdio>

using namespace std;

//In int main, I left all of my testing code to let you see my thought process in debugging.
//Many variables for strings and doubles were made to hold all of the different values that could be inputted from the txt file and how to manipulate 
//these inputs via flags. 
int main(){
string Enter;
string Name;
string String;
double Score = 0;
double doubleInput = -1; 
double Average = 0;
double averageAmount = 0;
int flagName = 0;
int flagAverage = 0;

//Use this line to read the text until there is nothing to read.
while(!cin.eof()){

	//This if statement determines if the input is a double or not.  If not, the input goes into a string variable
	if(!(cin >> doubleInput)){
		cin.clear();
		cin >> Enter;
		//cout << "It read as Enter" << endl;
		doubleInput = -1;
	}
			  /*cout << endl;
			    cout << "Enter: " << Enter << endl;
				cout << "Score: " << Score << endl;
				cout << "Average: " << Average << endl;
			 */

	//To avoid too many comments, the rest of this while loop is made up of many if and else statements that are flags for the inputs.  The flags search for
	//if the input is the words "NAME" or "AVERAGE" and sets flags for the next inputs to either be put into variables or manipulated in some way 
	//(like being put in average or score).  Anything else that is not directly after NAME or not a double is ignored.
	if(doubleInput >= 0){
	//cout << "it read as double" << endl;
		if(flagAverage == 1){
			Average += doubleInput;
			averageAmount += 1;
			//cout << "average added" << endl;
			doubleInput = -1;
		}
		else{
		//cout << "it added" << endl;
			Score += doubleInput;
			doubleInput = -1;
		}
	}
	else if(flagName == 1){
			Name = Enter;
			flagName = 2;
		}
	else if(Enter == "NAME"){
			flagName += 1;
			if(averageAmount > 0){
			Score = Score + (Average / averageAmount);
			Average = 0;
			averageAmount = 0;
			flagAverage = 0;
			//cout << "Name was read" << endl;
			} 
		}
	else if(Enter == "AVERAGE"){
		flagAverage = 1;
		if(averageAmount > 0){
			Score = Score + (Average / averageAmount);
		}
		//cout << "its working" << endl;
		Average = 0;
		averageAmount = 0;
		}
	else{
		flagAverage = 0;
		if(averageAmount > 0){
			Score = Score + (Average / averageAmount);
			Average = 0;
			averageAmount = 0;
			//cout << "last part is going" << endl;
		}

	}
	Enter = "";
}

//This adds the average varaible to score if the file was done being read.
if(averageAmount > 0){
	Score = Score + (Average / averageAmount);
}

//Finally outputs the Name and Score
cout << Name << " " << Score << endl;
return 0;
}
