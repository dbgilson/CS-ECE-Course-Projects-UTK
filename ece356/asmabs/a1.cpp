#include <cstdio>

extern "C" {
	int asmarray(int arra[], int size);
}

int asmarray(int arr[], int size){
	int i;
	int result = 0;
	for (i = 0; i < size; i++){
		result += arr[i];
	}
	return result;
}

int main(){
	int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9 ,10};

	int result = asmarray(a, 10);

	print("Result = %d\n", result);
	return 0;
}
