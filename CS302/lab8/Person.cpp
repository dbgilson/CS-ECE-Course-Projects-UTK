//CS 302 Lab 8
//By: David Gilson
//This file contains the function definitions of the class member functions found in Person.h.
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
using namespace std;

#include "Person.h"

//This takes a type enumeration and returns its related string.
string type2str(type_enum t){
    switch(t){
		case FACULTY:
			return "Faculty";
		case ASST_PROF:
			return "Assistant Professor";
		case ASSOC_PROF:
			return "Associate Professor";
		case FULL_PROF:
			return "Full Professor";
		case STUDENT:
			return "Student";
		case FRESHMAN:
			return "Freshman";
		case SOPHOMORE:
			return "Sophomore";
		case JUNIOR:
			return "Junior";
		case SENIOR:
			return "Senior";
		case UNKNOWN:
			return "Unknown";
		default:
			return "Unknown";
	}
}

//PERSON CLASS MEMBER FUNCTION IMPLEMENTATION
person::person(){
    name = "";
    type = UNKNOWN;
}

person::person(const string &s, type_enum t){
    name = s;
    type = t;
}

ostream & operator<<(ostream &out, person &p){
    p.print_personinfo(out);
    p.print_courseinfo(out);
    return out;
}

//FACULTY CLASS MEMBER FUNCTION IMPLEMENTATION
faculty::faculty(){
    name = "";
    type = FACULTY;
}

faculty::faculty(const string& s, type_enum t){
    name = s;
    type = t;
}

faculty::~faculty(){
	delete &courselist;
	delete &name;
	delete &type;
}
void faculty::add_course(string &s){
    courselist.insert(s);
}

void faculty::print_personinfo(ostream &out){
    out << "      Name: " << "Dr. " << name << endl;
    out << "  Category: " << type2str(type) << endl;
}

void faculty::print_courseinfo(ostream &out){
    set<string>::iterator ii;

    for(ii = courselist.begin(); ii != courselist.end(); ii++){
        out << "    Course: " << *ii << endl;
    }
}

//STUDENT CLASS MEMBER FUNCTION IMPLEMENTATION
student::student(){
    name = "";
    type = STUDENT;
}

student::student(const string &s, type_enum t){
    name = s;
    type = t;
}

student::~student(){
    delete &courselistS;
	delete &name;
	delete &type;
}

void student::add_course(string &s, float &f){
    courselistS.insert(pair<string, float>(s, f) );
}

void student::print_personinfo(ostream &out){
    out << "      Name: " << name << endl;
    out << "  Category: " << type2str(type) << endl;
}

void student::print_courseinfo(ostream &out){
    map<string, float>::iterator ii;
    float gpa_total;
    size_t i;

    i = 0;
    gpa_total = 0.0f;

    for(ii = courselistS.begin(); ii != courselistS.end(); ii++, i++){
        //Course name
        out << "    Course: " << setw(25) << left << ii->first << " ";

        //GPA printing
        out << fixed << setprecision(2) << ii->second << " ";
        gpa_total += ii->second;
        out << fixed << setprecision(2) << (gpa_total / (i + 1));

        //End
        out << endl;
    }
}

