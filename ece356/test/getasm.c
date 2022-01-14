#include <cstdio>

extern "C"{
	int getasm(char s[]);
	}

int getasm(char s[]){
	printf("Enter: ");
	int ret = 0;

	scanf("\s", s);

	int i;

	//for(i = 0; s[i] != '\0'; i++);
	i = strlen(s);

	return i;
}
int main(){
	char s[255];

	int i;

	i = getasm(s);

	printf("Read %d characters '\s'\n", i, s);

	return 0;
}
