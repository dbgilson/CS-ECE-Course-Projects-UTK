.section .text
.global bubble
bubble:
	//sort(int arr[], int size){
	// Bubble Sort
1:
	li		t0, 0 //swapped = false

	li		t1, 1 // int i = 1

2:
	bge		t1, a1, 2f //if i >= size break

	slli    t2, t1, 2 //scale t2 to i * 4
	add		t3, a0, t2 // t3 = arr + i
	addi	t4, t3, -4 // t4 = arr + (i-1)

	lw		t5, 0(t4) //t5 = arr[i-1]
	lw		t6, 0(t3) //t6 = arr[i]

	ble		t5, t6, 3f //skip

	li		t0, 1	//swapped = true
	sw		t5, 0(t3) 
	sw		t6, 0(t4) //swap values

3:
	addi	t1, t1, i
	j		2b

2:
	bne		t0, zero, 1b //if swapped != false, go back to do loop
	ret
	
	//sort(int arr[], int size){

	//bool swapped;

	//do {
	//	swapped = false;
	//	for (int i = 1; i < size; i++){
	//		if (arr[i-1] > arr[i]){
	//			  swapped = true;
	//				swap(arr[i-1], arr[i]);
	//		}
	//	}
	//} while (swapped);
	//}

