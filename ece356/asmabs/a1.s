.section .text
.global asmarray
asmarray:
	#int asmarray(int arr[], int size){

	#int i;
	mv t1, zero

	#int result = 0;
	mv t0, zero
	
loop_head:
	#for(i = 0; i < size; i++){
	blt t1, a1, loop_end
	
	#result += arr[i];
	slli t0, t0, 4

    #i++
	addi t1, t1, 1;


loop_end:

	move a0, t1
	ret
###############################33
#a1.cpp solution
asmarray:
	li		t0, 0 # i = 0
	li		t1, 0 # result

asm_cond:
	bge t0, a1, asm_exit

	#result += arr[i]
	slli	t2, t0, 2
	add		t2, t2, a0
	lw		t3, 0(t2)
	add		t1, t1, t3

	addi	t0, t0, 1
	j		asm_cond

asm_exit:
	mv		a0, t1
