//CS302 Lab 8
//By: David Gilson
//This is the header file containing the person, faculty, and student class headers.  I also defined the operator< in this file for each of the classes.
#ifndef __PERSON_H__ 
#define __PERSON_H__

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <set>
#include <map>
using namespace std;

enum type_enum { UNKNOWN=-1, FACULTY=0,  ASST_PROF=1, ASSOC_PROF=2, FULL_PROF=3,
                 STUDENT=10, FRESHMAN=11, SOPHOMORE=12, JUNIOR=13, SENIOR=14 };

//Person Class
class person{
    public:
        person();
        person(const string&, type_enum = UNKNOWN);
		virtual ~person(){ ; }

        void set_name(string &s){
            name = s;
        }
        type_enum &get_type(){
			return type;
		}

        friend ostream &operator<<(ostream &, person &);
		
        bool operator<(const person &p) const{
            if((this->type >= STUDENT && p.type >= STUDENT) || (this->type < STUDENT && p.type < STUDENT)){
				return (this->name < p.name);
			}	
			else if((this->type >= STUDENT) && (p.type < STUDENT)){
				return false;
			}

			else if((this->type < STUDENT) && (p.type >= STUDENT)){
				return true;
			}
		
        }

    protected:
        string name;
        type_enum type;
        virtual void print_personinfo(ostream &)=0;
        virtual void print_courseinfo(ostream &)=0;
};

//Faculty Class
class faculty : public person{
    public:
        faculty();
        faculty(const string&, type_enum);
        ~faculty();
        void add_course(string&);
        bool operator<(const faculty &f) const{
			if(this->type == f.type){
				return (this->name < f.name);
			}
			else{
				return (this->type < f.type);
			}
        }

    private:
        set<string> courselist;

        void print_personinfo(ostream&);
        void print_courseinfo(ostream&);
};

//Student Class
class student : public person{
    public:
        student();
        student(const string&, type_enum);
        ~student();
        void add_course(string &, float &);
        bool operator<(const student &s) const{
            if(this->type == s.type){
				return (this->name < s.name);
			}
			else{
				return (this->type < s.type);
			}
        }

    private:
        map<string, float> courselistS;

        void print_personinfo(ostream&);
        void print_courseinfo(ostream&);
};

#endif

