#include <cstdio>

extern "C" {
	void sort(int arr[], int size);
}

void sort(int arr[], int size){
// Bubble Sort
   bool swapped;

   do {
	   swapped = false;
	   for (int i = 1; i < size; i++){
		   if (arr[i-1] > arr[i]){
			   swapped = true;
			   swap(arr[i-1], arr[i]);
		   }
	   }
   } while (swapped);
}
int main(){
	int arr[] = {7, 9, 3, 1, 10, 2, 4, 1, 1, 6, 2, 7};

	sort(arr, 12)

    for(int i = 0; i < 12; i++){
		printf("%d: %d\n", i, arr[i]);

	}

	return 0;

}
