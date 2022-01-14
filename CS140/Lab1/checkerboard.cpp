// CS140 Lab1 Checkerboard By: David Gilson
// This code is meant to take 5 inputs from cin and create a checkerboard-like pattern with the values given.

//First we include the libraries and using namespace.
#include <iostream>
#include <iomanip>
using namespace std;

//In int main, we create the variables for rows, columns, starting character, cycle size, and width.  We read these variables in from cin, and if any of 
//the parameters other than the starting character is less than or equal to 0, we stop the program.
int main() {
	int R = 0;
	int C = 0;
	char SC, Element;
	int CS = 0;
	int W = 0;
	

	cin >> R >> C >> SC >> CS >> W;

	if(R <= 0 || C <= 0  || CS <= 0 || W <= 0 || (SC + CS) > 127){
		return 0;
	}

	//This is a row/column loop using for loops.  The only things added to it is the element formula to determine what element is being placed and the 
	//width variable which affects both the "width" and "height" of the element by effectively multiplying the rows and columns. The end result is 
	//a checkerboard like pattern printed out onto the console.
	for(int i = 0; i < R; i++){

		for(int l = 0; l < W; l++){

			for(int j = 0; j < C; j++){
				Element = SC + (i + j)%CS;

				for(int k = 0; k < W; k++){
					cout << Element;
				}
			}
		cout << endl;
		}
	}
	return 0;
}
