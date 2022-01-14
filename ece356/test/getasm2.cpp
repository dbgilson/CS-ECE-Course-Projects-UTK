#include <cstdio>

extern "C" {
	void getasm(int s[], int size, int a);
	void getasm2(int s[], int size);
}

void getasm(int s[], int size, int a){
	for(int i = 0; i < size; i++){
		s[i] *= a;
	}
}

void getasm2(int s[], int size){
	int result;
	for(int i = 0; i < size; i++){
		result += s[i];
	}

	printf("%d\n", result);

}

int main(){
	int s[] = {5, 4, 3, 2, 1};

	getasm(s, 5, 10);

	getasm2(s, 5);

	return 0;
}
