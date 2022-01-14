//Here are the assembly versions of the functions in lab5.  Descriptions of the functions are on the c++ file.  I included the c++ functions for reference.

//We always start assembly with this statement to ensure we are using the right syntax
.intel_syntax noprefix

//Now we go into the .text section because we are making functions to be called on by other programs.
.section .text
.global BubbleSort
BubbleSort:
/*void c_BubbleSort(char *array, int size){
	    char temp;
		    
		for(int i = 0; i < size; i++){
		for(int j = 0; j < (size-i-1); j++){
		temp = array[j];
		array[j] = array[j+1];
		array[j+1] = temp;
		}
	}
}
*/

//counters
mov	ecx, 0 //i
mov r8d, 0 //j

1:
//1st for loop
cmp	ecx, esi
	//leave loop when i >= size
jge	4f

2:
//2nd for loop
//make (size - i - 1)
mov r10d, esi
sub r10d, 1
sub r10d, ecx

//if j>= (size - i - 1), then go to previous for loop 
cmp r8d, r10d
jge 1b

3:
//temp = array[j]
movsx	r8, r8d
mov	r9b, [rdi + r8]

//increase j by 1
add r8d, 4

//create array[j+1] , array[j] = array[j+1]
movsx r10, r8d
mov r11b, [rdi + r10]
mov [rdi + r8], r11b

//array[j+1] = temp
mov	[rdi + r10], r9b

//increment i by 1 and restart 2nd for loop
inc ecx
jmp 2b

4:
ret



.global SelectionSort
SelectionSort:
/*void c_SelectionSort(short *array, int size){
	for (int i = 0; i < size; ++i){
	for (int j = i+1; j < size; ++j){
		if (array[i] > array[j]){
		array[i] = array[i]+array[j];
		array[j] = array[i]-array[j];
		array[i] = array[i]-array[j];
		}	
	}
 }	
}*/

//i = 0, j = i + 1
mov ecx, 0 //i
mov r8d, 1 //j
add r8d, ecx

1:
//first for loop
//if i >= size, end function
cmp	ecx, esi
jge 4f

2:
//Second For loop
//if j >= size, go to previous for loop
cmp r8d, esi
jge 1b

//offsets
movsx	r8, ecx
movsx	r9, r8d

//i++
inc ecx

//array[j]
mov r12w, [rdi + 2*r9]

//if array[i] > array[j], go into 2nd for loop
cmp [rdi + 2*r8],r12w
jg 3f


3:

//array[i]+array[j]
add [rdi + 2*r8], r12w
mov r10w, [rdi + 2*r8]
sub [rdi + 2*r8], r12w

//array[i]-array[j]
sub [rdi + 2*r8], r12w
mov r11w, [rdi + 2*r8]
add [rdi + 2*r8], r12w

mov [rdi+2*r8], r10w

mov [rdi +2*r9], r11w

mov [rdi + 2*r8], r11w

//j++
inc r8d

jmp 2b

4: 
ret



.global InsertionSort
InsertionSort:
/*void c_InsertionSort(int *array, int size){
	   int j, temp;
	      
		for (int i = 0; i < size; i++){
		j = i;
		while (j > 0 && array[j] < array[j-1]){
		temp = array[j];
		array[j] = array[j-1];
		array[j-1] = temp;
		j--;
		}
	}
}*/
//i = 0, set temp, set j
mov	ecx, 0
mov	r9d, 0
mov r10d, 0

1:
//1st for loop

//if i >= size, end loop
cmp	ecx, esi
jge 5f

//j = i
mov r10d, ecx

2:
//Enter While loop

//if j > 0, go to next check
cmp	r10d, 0
jg 3f

inc ecx
//if not, go back to for loop
jmp 1b

3:
//if array[i] < array[j-1], go into loop

//make register for j-1
mov r11d, r10d
sub	r11d, 4

movsx	r8, ecx
movsx	r9, r11d
movsx	r10, r10d

mov	r12d, [rdi + 4*r8]//array[i]
mov r13d, [rdi + 4*r9]//array[j-1]

//compare them
cmp	r12d, r13d
jl 4f

inc ecx
//jump back to for loop if not
jmp 1b

4:
//temp = array[j]
mov r9d, [rdi + 4*r10]

//array[j] = array[j-1]
mov	[rdi + 4*r10], r13d

//array[j-1] = temp
mov [rdi + 4*r9], r9d

//j--
dec r10d

5:
ret



.global GreatestSum
GreatestSum:
/*long c_GreatestSum(int *array, int size){
	long sum = 0;
	c_InsertionSort(array, size);
			    
	for(int i = 0; i < (size - 1); i++){
		sum += array[i];
	}
	sum += (array[size - 1] * 2);
	return sum;
}*/

//int i = 0, long sum = 0
mov	ecx, 0 //i
mov r10, 0 //sum

//call insertion sort to sort the array
call InsertionSort


//start for loop
1:
//set register for size - 1
mov r11d, esi
dec r11d

//if i >= size - 1, then leave for loop
cmp ecx, r11d
jge 3f

2:
//sum += array[i] (Multiply by 8 since it is a long)
movsx r8, ecx
mov r11, [rdi + 8*r8]

inc ecx
jmp 1b

3:
//sum += (array[size-1] * 2)
movsx r9, esi
dec r9
mov r12, [rdi + 4*r9]

imul r12, 2
add r10, r12

//move sum into ax, then return
mov rax, r10
ret




