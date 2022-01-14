//CS 302 Lab 8
//By: David Gilson
//The purpose of this code is to read an xml file of given student and faculty information and use inheritance based programing to create classes for either a student or faculty
//member.  We then take this information and output it in a given format.  This is the int main file where we read in the xml file and process the information.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>
#include <set>
#include <iterator>
#include <map>
using namespace std;

#include "Person.h"
#include "Sptrsort.h"

//This function takes in a string and returns a type enumeration.
type_enum str2type(string &s){
		if(s == "Faculty")
			return FACULTY;
		else if(s == "Assistant Professor")
			return ASST_PROF;
		else if(s == "Associate Professor")
			return ASSOC_PROF;
		else if(s == "Full Professor")
			return FULL_PROF;
		else if(s == "Student")
			return STUDENT;
		else if (s == "Freshman")
			return FRESHMAN;
		else if (s == "Sophomore")
			return SOPHOMORE;
		else if (s == "Junior")
			return JUNIOR;
		else if (s == "Senior")
			return SENIOR;
		else
			return UNKNOWN;
}

//In int main, we read each line of the xml file and extract information to create our "person" to push back onto a list.  This person goes onto a general person
//list, as well as either a student or faculty list.  We can then use smart pointer code to sort the lists how we want them and display the list given a specific
//input.
int main(int argc, char *argv[]) {
  //if (argc/argv problem) {
    //output error message
	//return 0;
  //}

  if(argc != 2){
	  cout << "usage: ./Data_processor -person|-faculty|-student < data.xml" << endl;
	  return 0;
  }
  
  string command = argv[1];
  if(command != "-person" && command != "-faculty" && command != "-student"){
	  cout << "usage: ./Data_processor -person|-faculty|-student < data.xml" << endl;
	  return 0;
  }

  person *n_person;
  faculty *f_person;
  student *s_person;

  vector<person *> person_list;
  vector<faculty *> faculty_list;
  vector<student *> student_list;

  type_enum type = UNKNOWN;
  type_enum cat = UNKNOWN;

  int line = 1;
  int i;
  size_t iL, iR;
  string input, temp, temp2;
  float tempf;

  //CS307: CHECK CORRECTESS OF XML HEADER 

  while (getline(cin, input)) {
	++line;

	if ((iL = input.find('<')) == string::npos) {
	  continue;

    } else if (input.compare(iL,9,"<faculty>") == 0) {
	  //set type 
      type = FACULTY;

	  n_person = new faculty("", type);
	  f_person = static_cast<faculty *>(n_person);
	  s_person = NULL;
//	  course.clear(); 	  

	} else if (input.compare(iL,10,"</faculty>") == 0){
	  
	  person_list.push_back(n_person);
	  faculty_list.push_back(f_person);

	  //reset type
	  type = UNKNOWN;
	  //reset subtype
	  continue;

	} else if (input.compare(iL,9,"<student>") == 0) {
	  //set type
	  type = STUDENT;

	  n_person = new student("", type);
	  f_person = NULL;
	  s_person = static_cast<student *>(n_person);
//	  course.clear();
//	  gp.clear();

	} else if (input.compare(iL,10,"</student>") == 0) {

	  person_list.push_back(n_person);
	  student_list.push_back(s_person);
	  //reset type
	  type = UNKNOWN;
	  //reset subtype
	  continue;

	} else if (input.compare(iL,5,"<name") == 0) {
	  iL = input.find("=\"", iL);
	  iR = input.find("\"/>", iL+2);

	  temp = input.substr(iL+2, (iR - iL)-2);
	  n_person->set_name(temp);

	} else if (input.compare(iL,9,"<category") == 0) {
	  iL = input.find("=\"", iL);
	  iR = input.find("\"/>", iL+2);
		
	  temp = input.substr(iL+2, (iR-iL)-2);
	  cat = str2type(temp);
	  n_person->get_type() = cat;

	} else if (input.compare(iL,7,"<course") == 0) {
	  iL = input.find("=\"", iL);
	  iR = input.find("\"", iL+2);
	  
	  temp = input.substr(iL+2, (iR-iL)-2);

	  iL = iR;
	  if (type == FACULTY) {
	    iR = input.find("/>", iL+1);
		
		//Add Course to Faculty
		f_person->add_course(temp);

	  } else if (type == STUDENT) {
	    iL = input.find("gp=\"", iL);
	    iR = input.find("\"/>", iL+4);

		temp2 = input.substr(iL+4, (iR - iL)-4);

		stringstream ss(temp2);
		if(ss >> tempf){
			//Add Course and GPA to Student
			s_person->add_course(temp, tempf);
		}
		else{
			cout << "Error in reading gp" << endl;
			return 0;
		}

	  }
	}
  }

 // EXTRACT LIST OF POINTERS TO PERSON, FACULTY OR STUDENT
 // ENTRY, SORT USING SPTRSORT AND PRINT RESULT TO STDOUT

 // NOTE: PERSON, FACULTY AND STUDENT EACH HAVE THEIR OWN
 // OVERLOADED COMPARISON OPERATOR (--> DIFFERENT SORTING)

 if(command == "-person"){
	sptrsort(person_list);
	for(i = 0; i < person_list.size(); i++){
		 cout << *person_list[i] << endl;
	}
 }
 else if(command == "-faculty"){
	 sptrsort(faculty_list);
	 for(i = 0; i < faculty_list.size(); i++){
		 cout << *faculty_list[i] << endl;
	 }
 }
 else if(command == "-student"){
	 sptrsort(student_list);
	 for(i = 0; i < student_list.size(); i++){
		 cout << *student_list[i] << endl;
	 }
 }

 // RELEASE ALL DYNAMICALLY ALLOCATED MEMORY
 
 student_list.clear();
 faculty_list.clear();

 //Could also just do person_list.clear(), but I wasn't too sure?
}
