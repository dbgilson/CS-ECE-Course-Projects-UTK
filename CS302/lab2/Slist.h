//CS302 Lab 2
//By: David Gilson
//This is the header file for Slist_usage.cpp.  This file contains class data for the slist class, smart pointers, and needed functions and overloaded
//operators for each class.  Most of this code was given to us, but we were meant to modify the node struct, create the sptr subclass, and create the
//slist sort function.
#ifndef SLIST_H
#define SLIST_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

template <class T>
class slist {
  private:
    struct node {
      node() { data = T(); next = NULL; }
      //This is the node constructor that takes a data key.
	  node(const T &key){
		data = key;
		next = NULL;
	  }
      //This is the overloaded < operator to allow us to compare node data.
	  bool operator<(const node &r) const{
		return data < r.data;
	  }
      T data;
      node *next;
    };

    //This is the smart pointer sub-class.  Instead of being template based, it is now node based.
    class sptr {
	  public:
	    sptr(node *_ptr=NULL){
		  ptr = _ptr;
	    }
	    bool operator< (const sptr &rhs) const {
		  return *ptr < *rhs.ptr;
        }
	    operator node * () const {
		  return ptr;
	    }
	  private:
	    node *ptr;
    };

  public:
	class iterator {
	  public:
	    iterator() { p = NULL; }
	    T & operator*() { return p->data; }
	    iterator & operator++() { p = p->next; return *this; }
	    bool operator!=(const iterator & rhs) const { return p != rhs.p; }

	  private:
	    iterator(node *n_p) { p = n_p; }
	    node *p;

	  friend class slist<T>;
	};

  public:
    slist();
	~slist();

    void push_back(const T &);
	void sort(const string &);

	iterator begin() { return iterator(head->next); }
	iterator end() { return iterator(NULL); }

  private:
	node *head;
	node *tail;
};

template <typename T>
slist<T>::slist() {
  head = new node();
  tail = head;
}

template <typename T>
slist<T>::~slist() {
  while (head->next != NULL) {
    node *p = head->next;
    head->next = p->next;
    delete p;
  }
  delete head;

  head = NULL;
  tail = NULL;
}

template <typename T>
void slist<T>::push_back(const T &din) {
  tail->next = new node(din);
  tail = tail->next;
}

//Here is the slist sort function.  Given a name to choose which sorting algorithm to use, it counts the elements in the given vector, puts the values into a sptr
//vector, sorts it, and then rearranges the sorted vector to give the sorted values.
template <typename T>
void slist<T>::sort(const string &algname) {
	//Element counting
	int num = 0;
	node *p = head;
	while(p != NULL){
		num++;
		p = p->next;
	}
	
	//Vector allocation
	vector<sptr> Ap(num-1);

	p = head;
	for(int i = 0; i < num; i++){
		Ap[i] = p->next;
		p = p->next;
	}
	
	//Sorting algorithm choice
	if(algname.compare("-quicksort") == 0){
		std::sort(Ap.begin(), Ap.end());
	}
	else{
		std::stable_sort(Ap.begin(), Ap.end());
	}
	
	//Rearranging
	p = head;
	for(int j = 0; j < num-1; j++){
		p->next = Ap[j];
		p = p->next;
	}
	p->next = NULL;
} 
#endif // SLIST_H
