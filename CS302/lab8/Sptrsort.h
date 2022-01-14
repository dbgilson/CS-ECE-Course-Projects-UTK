#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>


template <typename T>
class sptr {
  public:
    sptr(T *_ptr=NULL) { ptr = _ptr; }

    bool operator< (const sptr &rhs) const {
      return *ptr < *rhs.ptr;    
	}

    operator T * () const {return ptr; }
  private:
    T *ptr;
};

template <typename T>
void data2ptr(vector<T> &A, vector < sptr<T> > &Ap) {
  Ap.resize(A.size());
  for (int i=0; i<A.size(); i++)
    Ap[i] = &A[i];
}

template <typename T>
void ptr2data(vector<T> &A, vector < sptr<T> > &Ap) {
  int i, j, nextj;
  for (i=0; i<A.size(); i++) {
    if (Ap[i] != &A[i]) {
      T tmp = A[i];
      for (j=i; Ap[j] != &A[i]; j=nextj) {
        nextj = Ap[j] - &A[0];
        A[j] = *Ap[j];
        Ap[j] = &A[j];
      }
      A[j] = tmp;
      Ap[j] = &A[j];
    }
  }
}

template <typename T>
void sptrsort(std::vector<T *> &A) {
    size_t a_sz, i; 
	vector < sptr<T> > ptrs;

	//Vector of smart pointers
	a_sz = A.size();
	ptrs.resize(a_sz);

	//Fill with values from "A"
	for(i = 0; i < a_sz; i++){
		ptrs[i] = sptr<T>(A[i]);
	}

	//Introsort
	std::sort(ptrs.begin(), ptrs.end());

	//Store sorted values into "A"
	for(i = 0; i < a_sz; i++){
		A[i] = ptrs[i];
	}
}
