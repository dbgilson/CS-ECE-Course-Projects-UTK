#include <cstdio>

struct MyStruct{
	char a;
	char b;
	int *c;
}

extern "C" {
	void doasm(MyStruct s[], int size);
}

ovid doesasm(MyStruct s[], int size){
	int i;
	for (i = 0; i < size; i++){
		*s[i].c = s[i].a + s[i].b;
	}
}

int main() {
	MyStruct s[] = {
		{2, 1, new int},
		{4, 3, new int},
		{6, 5, new int},
		{8, 7, new int},
	};
	
	int i;

	doasm(s, 4);

	for (i = 0; i < 4; i++){
		printf("%d + %d = %d\n", s[i].a, s[i].b, *(s[i].c));
	}
	return 0;
}
